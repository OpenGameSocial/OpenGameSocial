#include "MacHttpRequest.h"

#import <Foundation/Foundation.h>

void HttpRequest(const std::string &url,
                 const std::function<void(const std::string &response, const std::string &error)> &callback) {
    @autoreleasepool {
        NSString *nsUrl = [NSString stringWithUTF8String:url.c_str()];
        NSURL *requestUrl = [NSURL URLWithString:nsUrl];

        // Создаем задачу с блоком (асинхронно)
        NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithURL:requestUrl
                                                                 completionHandler:^(NSData *data,
                                                                                     NSURLResponse *response,
                                                                                     NSError *error) {
                                                                     if (error) {
                                                                         // Передаем ошибку в коллбек
                                                                         callback("", std::string(
                                                                                 error.localizedDescription.UTF8String));
                                                                         return;
                                                                     }

                                                                     if (data) {
                                                                         NSString *responseData = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                                                                         callback(std::string(responseData.UTF8String),
                                                                                  "");
                                                                     } else {
                                                                         callback("", "Empty response");
                                                                     }
                                                                 }];

        [task resume]; // Стартуем асинхронный запрос
    }
}
