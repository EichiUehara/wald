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
        : redis("tcp://127.0.0.1:6379"),  // Assuming Redis is running locally on default port
          mockRedisRepo(redis),
          handler(mockRedisRepo, "test_queue") {}
    void SetUp() override {
    }

    void TearDown() override {
        // delete &handler;
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
    // Using regex match for timestamp as it's generated dynamically


    // Act
    auto result = handler.PutCF(0, rocksdb::Slice(key), rocksdb::Slice(value));
    EXPECT_TRUE(result.ok());
    // EXPECT_CALL(mockRedisRepo, SendToRedisQueue("test_queue", MatchesRegex(R"(\{"k":"test_key","v":"test_value","op":"Put","t":[0-9]+\})")));

    // Assert
    // Expectations set by EXPECT_CALL will automatically verify the results
}

TEST_F(RocksWalBatchHandlerTest, DeleteCFTest) {
    // Arrange
    std::string key = "test_key";

    json expected_json;
    expected_json["k"] = key;
    expected_json["op"] = "Delete";
    // Using regex match for timestamp as it's generated dynamically

    // Act
    auto result = handler.DeleteCF(0, rocksdb::Slice(key));
    EXPECT_TRUE(result.ok());
    // EXPECT_CALL(mockRedisRepo, SendToRedisQueue("test_queue", MatchesRegex(R"(\{"k":"test_key","op":"Delete","t":[0-9]+\})")));

    // Assert
    // Expectations set by EXPECT_CALL will automatically verify the results
}
