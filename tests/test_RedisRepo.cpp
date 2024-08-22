#include <gtest/gtest.h>
#include <sw/redis++/redis++.h>
#include "RedisRepo.h"

using namespace sw::redis;

class RedisRepoIntegrationTest : public ::testing::Test {
protected:
    Redis redis;
    RedisRepo repo;

    RedisRepoIntegrationTest()
        : redis("tcp://127.0.0.1:6379"), // Adjust if necessary
          repo(redis) {}

    void SetUp() override {
        // Optional: Clear Redis database before each test
        redis.flushall();
    }

    void TearDown() override {
        // Optional: Clear Redis database after each test
        redis.flushall();
    }
};

TEST_F(RedisRepoIntegrationTest, SendToRedisQueue_Success) {
    std::string queue = "test_queue";
    std::string message = "test_message";

    // Call the method
    repo.SendToRedisQueue(queue, message);
    // Retrieve the message directly from Redis to verify
    std::vector<std::string> result;
    redis.lrange(queue, 0, -1, std::back_inserter(result));
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], message);
}

TEST_F(RedisRepoIntegrationTest, GetFromRedisQueue_ReturnsMessage) {
    std::string queue = "test_queue";
    std::string message = "test_message";

    // Add a message directly to Redis
    redis.lpush(queue, message);

    // Call the method
    std::string result = repo.GetFromRedisQueue(queue);

    // Verify the result
    EXPECT_EQ(result, message);
}

TEST_F(RedisRepoIntegrationTest, GetFromRedisQueue_ReturnsEmptyStringWhenNoMessage) {
    std::string queue = "test_queue";

    // Call the method
    std::string result = repo.GetFromRedisQueue(queue);

    // Verify the result
    EXPECT_EQ(result, "");
}
