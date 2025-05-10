#include "MacHttpRequest.h"

#import <Foundation/Foundation.h>
#include "Core/Http/HttpManager.h"


using namespace OGS::Http::Mac;

bool CMacHttpRequest::Run(const std::shared_ptr<CHttpRequest>& Owner)
{
    if (!Url.IsValid() || Method.empty())
    {
        return false;
    }

    @autoreleasepool
    {
        NSString *nsUrl = [NSString stringWithUTF8String:Url.ToString().c_str()];
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:nsUrl]];

        NSString *httpMethod = [NSString stringWithUTF8String:Method.c_str()];
        [request setHTTPMethod:httpMethod];

        for (const auto& Header : Headers)
        {
            NSString *HeaderKey = [NSString stringWithUTF8String:Header.first.c_str()];
            NSString *HeaderValue = [NSString stringWithUTF8String:Header.second.c_str()];
            [request setValue:HeaderValue forHTTPHeaderField:HeaderKey];
        }

        if (!Body.empty())
        {
            NSData *postData = [NSData dataWithBytes:Body.c_str() length:Body.size()];
            [request setHTTPBody:postData];
        }

        NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request
                                                                     completionHandler:^(NSData *data,
                                                                                         NSURLResponse *response,
                                                                                         NSError *error) {
            auto& Mgr = CHttpManager::Get();
            
            int statusCode = -1;
            
            if ([response isKindOfClass:[NSHTTPURLResponse class]])
            {
                NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                statusCode = (int)[httpResponse statusCode];
            }
            
            if (error)
            {
                Mgr.CompleteRequest(Guid, {
                    statusCode,
                    std::string(error.localizedDescription.UTF8String)
                });
                
                return;
            }
            
            if (data)
            {
                NSLog(@"Raw data: %@", data);

                const void* Bytes = [data bytes];
                size_t Size = [data length];

                Mgr.CompleteRequest(Guid, {
                    statusCode,
                    std::string(reinterpret_cast<const char*>(Bytes), Size)
                });
                
                return;
            }
            
            Mgr.CompleteRequest(Guid, {
                statusCode,
                {}
            });
        }];

        auto& Mgr = CHttpManager::Get();
        Mgr.AddRequest(Owner);

        [task resume];
    }

    return true;
}
