#include <gtest/gtest.h>
#include <rocksdb/db.h>
#include "RocksRepo.h"

class RocksRepoTest : public ::testing::Test {
protected:
    RocksRepo* repo;
    rocksdb::DB* db;
    std::string db_path;

    void SetUp() override {
        // Set up a temporary database for testing
        db_path = "/tmp/testdb";
        rocksdb::Options options;
        options.create_if_missing = true;

        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        ASSERT_TRUE(status.ok());

        repo = new RocksRepo(db);
    }

    void TearDown() override {
        // Clean up
        delete repo;
        delete db;
        rocksdb::DestroyDB(db_path, rocksdb::Options());
    }
};

TEST_F(RocksRepoTest, PutAndGet) {
    std::string key = "test_key";
    std::string value = "test_value";

    // Use the put method to store a key-value pair
    repo->put(key, value);

    // Retrieve the value using the get method and verify it
    std::string retrieved_value = repo->get(key);
    EXPECT_EQ(retrieved_value, value);
}

TEST_F(RocksRepoTest, GetNonExistentKey) {
    std::string non_existent_key = "non_existent_key";

    // Attempt to retrieve a value for a non-existent key
    std::string value = repo->get(non_existent_key);

    // Verify that the returned value is "0" as per the current implementation
    EXPECT_EQ(value, "0");
}

TEST_F(RocksRepoTest, DeleteKey) {
    std::string key = "test_key";
    std::string value = "test_value";

    // First, put a key-value pair
    repo->put(key, value);

    // Ensure the value is stored
    EXPECT_EQ(repo->get(key), value);

    // Now, delete the key
    repo->del(key);

    // Verify that the key is no longer retrievable
    std::string retrieved_value = repo->get(key);
    EXPECT_EQ(retrieved_value, "0");
}
