#include <gtest/gtest.h>
#include <sw/redis++/redis++.h>
#include "RedisRepo.h"

using namespace sw::redis;

class RedisRepoIntegrationTest : public ::testing::Test {
protected:
    Redis redis;
    RedisRepo repo;

    RedisRepoIntegrationTest()
        : redis("tcp://127.0.0.1:6379"),
          repo(redis) {}

    void SetUp() override {
        redis.flushall();
    }

    void TearDown() override {
        redis.flushall();
    }
};

TEST_F(RedisRepoIntegrationTest, SendToRedisQueue_Success) {
    std::string queue = "test_queue";
    std::string message = "test_message";

    repo.SendToRedisQueue(queue, message);
    std::vector<std::string> result;
    redis.lrange(queue, 0, -1, std::back_inserter(result));
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], message);
}

TEST_F(RedisRepoIntegrationTest, GetFromRedisQueue_ReturnsMessage) {
    std::string queue = "test_queue";
    std::string message = "test_message";

    redis.lpush(queue, message);

    std::string result = repo.GetFromRedisQueue(queue);

    EXPECT_EQ(result, message);
}

TEST_F(RedisRepoIntegrationTest, GetFromRedisQueue_ReturnsEmptyStringWhenNoMessage) {
    std::string queue = "test_queue";

    std::string result = repo.GetFromRedisQueue(queue);

    EXPECT_EQ(result, "");
}
