#include "RedisRepo.h"
#include <iostream>
void RedisRepo::SendToRedisQueue(const std::string queue, const std::string message) {
    auto result = this->redis.lpush(queue, message);
}

std::string RedisRepo::GetFromRedisQueue(const std::string queue) {
    auto result = this->redis.blpop(queue, std::chrono::seconds(1));
    if (result) {
        auto pair = result.value();
        return pair.second;
    } else {
        auto retry = 0;
        try {
            while (retry < 3) {
                result = this->redis.blpop(queue, std::chrono::seconds(1));
                if (result) {
                    auto pair = result.value();
                    return pair.second;
                }
                retry++;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        return "";
    }
}
