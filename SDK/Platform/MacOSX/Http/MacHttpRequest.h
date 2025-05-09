#pragma once

#include <string>
#include <functional>


void HttpRequest(const std::string &url,
                 const std::function<void(const std::string &response, const std::string &error)> &callback);