#include <gtest/gtest.h>
#include <rocksdb/db.h>
#include "RocksRepo.h"

class RocksRepoTest : public ::testing::Test {
protected:
    RocksRepo* repo;
    rocksdb::DB* db;
    std::string db_path;

    void SetUp() override {
        db_path = "/tmp/testdb";
        rocksdb::Options options;
        options.create_if_missing = true;

        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        ASSERT_TRUE(status.ok());

        repo = new RocksRepo(db);
    }

    void TearDown() override {
        delete repo;
        delete db;
        rocksdb::DestroyDB(db_path, rocksdb::Options());
    }
};

TEST_F(RocksRepoTest, PutAndGet) {
    std::string key = "test_key";
    std::string value = "test_value";

    repo->put(key, value);

    std::string retrieved_value = repo->get(key);
    EXPECT_EQ(retrieved_value, value);
}

TEST_F(RocksRepoTest, GetNonExistentKey) {
    std::string non_existent_key = "non_existent_key";

    std::string value = repo->get(non_existent_key);

    EXPECT_EQ(value, "0");
}

TEST_F(RocksRepoTest, DeleteKey) {
    std::string key = "test_key";
    std::string value = "test_value";

    repo->put(key, value);

    EXPECT_EQ(repo->get(key), value);

    repo->del(key);

    std::string retrieved_value = repo->get(key);
    EXPECT_EQ(retrieved_value, "0");
}
