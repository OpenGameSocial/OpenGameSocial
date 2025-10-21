#include "LinuxHttpRequest.h"

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "Core/Http/HttpManager.h"


class CCurlMultiManager
{
public:
    static CCurlMultiManager& Get()
    {
        static CCurlMultiManager Instance;
        return Instance;
    }

    bool AddHandle(CURL* InCurl, OGS::Http::Linux::CCurlHttpRequest* InRequest)
    {
        if (!InCurl || !InRequest)
        {
            return false;
        }

        {
            std::lock_guard Lock(Mutex);
            HandleToRequest[InCurl] = InRequest;

            if (curl_multi_add_handle(MultiHandle, InCurl) != CURLM_OK)
            {
                HandleToRequest.erase(InCurl);
                return false;
            }
        }

        Cv.notify_one();
        return true;
    }

    void RemoveHandle(CURL* InCurl)
    {
        if (!InCurl)
        {
            return;
        }

        std::lock_guard Lock(Mutex);

        HandleToRequest.erase(InCurl);
        curl_multi_remove_handle(MultiHandle, InCurl);
    }

private:
    CCurlMultiManager()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        MultiHandle = curl_multi_init();

        Running = true;

        WorkerThread = std::thread([this]
        {
            this->ThreadFunc();
        });
    }

    ~CCurlMultiManager()
    {
        Running = false;
        Cv.notify_one();

        if (WorkerThread.joinable())
        {
            WorkerThread.join();
        }

        if (MultiHandle)
        {
            curl_multi_cleanup(MultiHandle);
            MultiHandle = nullptr;
        }

        curl_global_cleanup();
    }

    void ThreadFunc()
    {
        while (Running)
        {
            int RunningHandles = 0;

            {
                std::lock_guard Lock(Mutex);
                if (MultiHandle)
                {
                    curl_multi_perform(MultiHandle, &RunningHandles);
                }
            }

            const CURLMsg* Message = nullptr;
            int MessagesInQueue = 0;

            while ((Message = curl_multi_info_read(MultiHandle, &MessagesInQueue)) != nullptr)
            {
                if (Message->msg != CURLMSG_DONE)
                {
                    continue;
                }

                CURL* CurlHandle = Message->easy_handle;
                const CURLcode ResultCode = Message->data.result;

                OGS::Http::Linux::CCurlHttpRequest* Request = nullptr;
                {
                    std::lock_guard Lock(Mutex);

                    if (auto Iterator = HandleToRequest.find(CurlHandle);
                        Iterator != HandleToRequest.end())
                    {
                        Request = Iterator->second;
                    }
                }

                long HttpCode = 0;
                curl_easy_getinfo(CurlHandle, CURLINFO_RESPONSE_CODE, &HttpCode);

                std::string ErrorString;

                if (ResultCode != CURLE_OK)
                {
                    ErrorString = OGS::Http::Linux::CCurlHttpRequest::GetFromCurlError(ResultCode);
                }

                if (Request)
                {
                    Request->OnCompleted(ResultCode, HttpCode, ErrorString);
                    RemoveHandle(CurlHandle);
                }
            }

            int DescriptorsCount = 0;
            int WaitResult = 0;
            {
                std::lock_guard Lock(Mutex);

                long TimeoutMs = 1000;
                curl_multi_timeout(MultiHandle, &TimeoutMs);

                if (TimeoutMs < 0)
                {
                    TimeoutMs = 1000;
                }

                WaitResult = curl_multi_wait(MultiHandle, nullptr, 0,
                                             static_cast<int>(TimeoutMs), &DescriptorsCount);
            }

            if (WaitResult != CURLM_OK)
            {
                std::unique_lock Lock(WaitMutex);
                Cv.wait_for(Lock, std::chrono::milliseconds(100));

                continue;
            }

            if (DescriptorsCount == 0)
            {
                std::unique_lock Lock(WaitMutex);
                Cv.wait_for(Lock, std::chrono::milliseconds(50));
            }
        }
    }

private:
    CURLM* MultiHandle = nullptr;
    std::thread WorkerThread;
    std::atomic<bool> Running = false;

    std::mutex Mutex;
    std::unordered_map<CURL*, OGS::Http::Linux::CCurlHttpRequest*> HandleToRequest;

    std::mutex WaitMutex;
    std::condition_variable Cv;
};

OGS::Http::Linux::CCurlHttpRequest::CCurlHttpRequest(const CGuid& InGuid) :
Guid(InGuid)
{}

OGS::Http::Linux::CCurlHttpRequest::~CCurlHttpRequest()
{
    if (EasyHandle)
    {
        CCurlMultiManager::Get().RemoveHandle(EasyHandle);

        curl_easy_cleanup(EasyHandle);
        EasyHandle = nullptr;
    }
}

void OGS::Http::Linux::CCurlHttpRequest::SetMethod(EHttpMethod InMethod)
{
    switch (InMethod)
    {
    case EHttpMethod::Get:
        Method = "GET";
        break;
    case EHttpMethod::Post:
        Method = "POST";
        break;
    case EHttpMethod::Put:
        Method = "PUT";
        break;
    case EHttpMethod::Delete:
        Method = "DELETE";
        break;
    case EHttpMethod::Patch:
        Method = "PATCH";
        break;
    }
}

void OGS::Http::Linux::CCurlHttpRequest::SetHeader(const std::string& Key, const std::string& Value)
{
    HeadersList.emplace_back(Key + ": " + Value);
}

size_t OGS::Http::Linux::CCurlHttpRequest::WriteCallback(void* InBuffer, size_t InSize, size_t InItemsCount,
                                                         void* InUserData)
{
    const size_t Total = InSize * InItemsCount;

    if (!InUserData)
    {
        return 0;
    }

    auto* Request = static_cast<CCurlHttpRequest*>(InUserData);
    Request->ResponseBody.append(static_cast<char*>(InBuffer), Total);

    return Total;
}

size_t OGS::Http::Linux::CCurlHttpRequest::HeaderCallback(char* InBuffer, size_t InSize, size_t InItemsCount,
                                                          void* InUserData)
{
    const size_t Total = InSize * InItemsCount;

    if (!InUserData)
    {
        return 0;
    }

    auto* Request = static_cast<CCurlHttpRequest*>(InUserData);
    Request->ResponseHeaders.append(InBuffer, Total);

    return Total;
}

std::string OGS::Http::Linux::CCurlHttpRequest::GetFromCurlError(CURLcode InCode)
{
    switch (InCode)
    {
    case CURLE_OK:
        return "No error";
    case CURLE_UNSUPPORTED_PROTOCOL:
        return "Unsupported protocol";
    case CURLE_COULDNT_CONNECT:
        return "Failed to connect to the server";
    case CURLE_COULDNT_RESOLVE_HOST:
        return "DNS name could not be resolved";
    case CURLE_OPERATION_TIMEDOUT:
        return "Request timed out";
    case CURLE_SSL_CONNECT_ERROR:
        return "SSL/TLS handshake failed";
    case CURLE_PEER_FAILED_VERIFICATION:
        return "SSL certificate validation failed";
    case CURLE_GOT_NOTHING:
        return "Server returned no data";
    case CURLE_SEND_ERROR:
        return "Failed to send data to server";
    case CURLE_RECV_ERROR:
        return "Failed to receive data from server";
    case CURLE_REMOTE_ACCESS_DENIED:
        return "Remote server refused access";
    case CURLE_TOO_MANY_REDIRECTS:
        return "HTTP redirect failed (too many redirects)";
    case CURLE_SSL_CACERT_BADFILE:
        return "Bad CA certificate";
    case CURLE_SSL_CERTPROBLEM:
        return "Client certificate problem";
    default:
        return std::string("Unknown curl error: ") + std::to_string(InCode);
    }
}

bool OGS::Http::Linux::CCurlHttpRequest::Run(const std::shared_ptr<CHttpRequest>& Owner)
{
    if (EasyHandle != nullptr)
    {
        return false;
    }

    EasyHandle = curl_easy_init();

    if (!EasyHandle)
    {
        return false;
    }

    const std::string& UrlStr = Url.ToString();
    curl_easy_setopt(EasyHandle, CURLOPT_URL, UrlStr.c_str());

    if (Method == "POST")
    {
        curl_easy_setopt(EasyHandle, CURLOPT_POST, 1L);
    }
    else if (Method == "PUT")
    {
        curl_easy_setopt(EasyHandle, CURLOPT_CUSTOMREQUEST, "PUT");
    }
    else if (Method == "DELETE")
    {
        curl_easy_setopt(EasyHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else if (Method == "PATCH")
    {
        curl_easy_setopt(EasyHandle, CURLOPT_CUSTOMREQUEST, "PATCH");
    }
    else
    {
        curl_easy_setopt(EasyHandle, CURLOPT_HTTPGET, 1L);
    }

    curl_slist* CurlHeaders = nullptr;

    for (const auto& Header : HeadersList)
    {
        CurlHeaders = curl_slist_append(CurlHeaders, Header.c_str());
    }

    if (CurlHeaders)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_HTTPHEADER, CurlHeaders);
    }

    if (!Body.empty())
    {
        curl_easy_setopt(EasyHandle, CURLOPT_POSTFIELDS, Body.c_str());
        curl_easy_setopt(EasyHandle, CURLOPT_POSTFIELDSIZE, static_cast<long>(Body.size()));
    }

    curl_easy_setopt(EasyHandle, CURLOPT_WRITEFUNCTION, &CCurlHttpRequest::WriteCallback);
    curl_easy_setopt(EasyHandle, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(EasyHandle, CURLOPT_HEADERFUNCTION, &CCurlHttpRequest::HeaderCallback);
    curl_easy_setopt(EasyHandle, CURLOPT_HEADERDATA, this);

    curl_easy_setopt(EasyHandle, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(EasyHandle, CURLOPT_CONNECTTIMEOUT, 10);

    curl_easy_setopt(EasyHandle, CURLOPT_FOLLOWLOCATION, 1);

    // const char* envClientCert = std::getenv("OGS_CLIENT_CERT");
    // const char* envClientKey  = std::getenv("OGS_CLIENT_KEY");
    // const char* envClientKeyPass = std::getenv("OGS_CLIENT_KEY_PASS");
    // const char* envClientCertType = std::getenv("OGS_CLIENT_CERT_TYPE"); // "PEM" or "P12"
    //
    // if (envClientCert)
    // {
    //     // If cert type is P12, tell curl
    //     if (envClientCertType && std::string(envClientCertType) == "P12")
    //     {
    //         curl_easy_setopt(EasyHandle, CURLOPT_SSLCERTTYPE, "P12");
    //     }
    //     else
    //     {
    //         // default to PEM
    //         curl_easy_setopt(EasyHandle, CURLOPT_SSLCERTTYPE, "PEM");
    //     }
    //
    //     curl_easy_setopt(EasyHandle, CURLOPT_SSLCERT, envClientCert);
    //
    //     if (envClientKey)
    //     {
    //         curl_easy_setopt(EasyHandle, CURLOPT_SSLKEY, envClientKey);
    //     }
    //
    //     if (envClientKeyPass)
    //     {
    //         curl_easy_setopt(EasyHandle, CURLOPT_KEYPASSWD, envClientKeyPass);
    //     }
    // }

    curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYHOST, 2L);

    curl_easy_setopt(EasyHandle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(EasyHandle, CURLOPT_PRIVATE, CurlHeaders);

    if (!CCurlMultiManager::Get().AddHandle(EasyHandle, this))
    {
        if (CurlHeaders)
        {
            curl_slist_free_all(CurlHeaders);
        }

        curl_easy_cleanup(EasyHandle);
        EasyHandle = nullptr;
        return false;
    }

    bIsCompleted = false;

    auto& Mgr = CHttpManager::Get();
    Mgr.AddRequest(Owner);

    return true;
}

void OGS::Http::Linux::CCurlHttpRequest::OnCompleted(CURLcode Result, long http_code, const std::string& ErrorString)
{
    ResponseCode = static_cast<int32_t>(http_code);

    bIsCompleted.store(true);

    void* Private = nullptr;
    curl_easy_getinfo(EasyHandle, CURLINFO_PRIVATE, &Private);

    if (Private)
    {
        auto* CurlHeaders = static_cast<struct curl_slist*>(Private);
        curl_slist_free_all(CurlHeaders);
    }

    curl_easy_cleanup(EasyHandle);
    EasyHandle = nullptr;

    if (Result != CURLE_OK)
    {
        const std::string Err = ErrorString.empty() ? GetFromCurlError(Result) : ErrorString;
        CHttpManager::Get().CompleteRequest(Guid, CHttpResponse(0, Err));
    }
    else
    {
        CHttpManager::Get().CompleteRequest(Guid, CHttpResponse(ResponseCode, ResponseBody));
    }
}
