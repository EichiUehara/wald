#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <RocksWalBatchHandler.h>
#include <RedisRepo.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace testing;

class MockRedisRepo : public RedisRepo {
public:
    MockRedisRepo(sw::redis::Redis& redis) : RedisRepo(redis) {}
    MOCK_METHOD(void, SendToRedisQueue, (const std::string queue, const std::string message));
};

class RocksWalBatchHandlerTest : public ::testing::Test {
protected:
    sw::redis::Redis redis;
    MockRedisRepo mockRedisRepo;
    RocksWalBatchHandler handler;
    RocksWalBatchHandlerTest()
        : redis("tcp://127.0.0.1:6379"),
          mockRedisRepo(redis),
          handler(mockRedisRepo, "test_queue") {}
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(RocksWalBatchHandlerTest, PutCFTest) {
    // Arrange
    std::string key = "test_key";
    std::string value = "test_value";

    json expected_json;
    expected_json["k"] = key;
    expected_json["v"] = value;
    expected_json["op"] = "Put";

    // Act
    auto result = handler.PutCF(0, rocksdb::Slice(key), rocksdb::Slice(value));
    EXPECT_TRUE(result.ok());
}

TEST_F(RocksWalBatchHandlerTest, DeleteCFTest) {
    // Arrange
    std::string key = "test_key";

    json expected_json;
    expected_json["k"] = key;
    expected_json["op"] = "Delete";

    // Act
    auto result = handler.DeleteCF(0, rocksdb::Slice(key));
    EXPECT_TRUE(result.ok());
}
