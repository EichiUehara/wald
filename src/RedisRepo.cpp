#include "RedisRepo.h"
#include <iostream>
void RedisRepo::SendToRedisQueue(const std::string queue, const std::string message) {
    auto result = this->redis.lpush(queue, message);

    // if (result <= 100) {
    //     # ifdef DEBUG
    std::cout << "SendToRedisQueue: " << message << std::endl;
    //     # endif
    //     # ifdef DEBUG
    std::cout << "Queue size: " << result << std::endl;
    //     throw std::runtime_error("Queue size exceeded 100");
    //     # endif
    // }
    // printf("SendToRedisQueue: %d\n", result);
}

std::string RedisRepo::GetFromRedisQueue(const std::string queue) {
    # ifdef DEBUG
    // ## calculate queue size
    auto size = this->redis.llen(queue);
    std::cout << "Queue size: " << size << std::endl;
    # endif
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
