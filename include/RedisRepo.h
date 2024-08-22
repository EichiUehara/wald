#pragma once
#include <sw/redis++/redis++.h>

class RedisRepo {
public:
    RedisRepo(sw::redis::Redis& redis) : redis(redis) {}
    ~RedisRepo() = default;
    void SendToRedisQueue(const std::string queue, const std::string message);
    std::string GetFromRedisQueue(const std::string queue);
private:
    sw::redis::Redis& redis;
};
