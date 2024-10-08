#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <rocksdb/db.h>
#include <sw/redis++/redis++.h>
#include "RocksWalProcessor.h"
#include "RocksRepo.h"
#include "RedisRepo.h"
#include <gtest/gtest.h>

std::string generateRandomString(size_t length) {
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, characters.size() - 1);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += characters[dist(engine)];
    }

    return result;
}
void initializeRocksDB(const std::string& db_path) {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.WAL_ttl_seconds = 100;
    options.WAL_size_limit_MB = 5000;
    // options.write_buffer_size = 1000000;
    // options.target_file_size_base = 1000000;
    options.max_write_buffer_number = 2;
    options.level0_file_num_compaction_trigger = 2;
    options.level0_slowdown_writes_trigger = 2;
    options.level0_stop_writes_trigger = 2;
    options.max_background_compactions = 2;
    options.max_background_flushes = 2;
    size_t length = 1000; // Length of the random string

    std::string randomString = generateRandomString(length);
    
    auto status = rocksdb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open RocksDB: " + status.ToString());
    }
    for (int i = 1; i <= 20000; i++) {
        db->Put(rocksdb::WriteOptions(), "key" + std::to_string(i), randomString);
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    
    delete db;
}

void testRocksWalProcessor() {
    const std::string rocksdb_path = "./test_db";
    const std::string state_db_path = "./test_state_db";
    const std::string redis_queue_name = "test_queue";
    const std::string redis_host = "localhost";
    const std::string redis_port = "6379";
    const std::string rocksdb_walseq_state_key = "walseq";
    sw::redis::Redis redis("tcp://" + redis_host + ":" + redis_port);
    redis.del(redis_queue_name);
    std::thread init_thread([&rocksdb_path]() {
        initializeRocksDB(rocksdb_path);
    });
    init_thread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    RocksWalProcessor processor(redis_host, redis_port, rocksdb_path, state_db_path, rocksdb_walseq_state_key, redis_queue_name);
    std::thread processor_thread([&processor]() {
        processor.run();
    });
    processor_thread.detach();
    std::this_thread::sleep_for(std::chrono::seconds(20));
    auto array = std::vector<int>();
    auto repo = RedisRepo(redis);
    for (int i = 1; i <= 40000; i++) {
        auto redis_value = repo.GetFromRedisQueue(redis_queue_name);
            std::cout << "Redis Queue Content: " << redis_value << std::endl;
        if (redis_value == "") {
            std::cout << "Redis Queue is empty." << std::endl;
            continue;
        }
        size_t key_pos_start = redis_value.find("\"k\":\"") + 5;
        size_t key_pos_end = redis_value.find("\"", key_pos_start);
        std::string key = redis_value.substr(key_pos_start, key_pos_end - key_pos_start);
        size_t pos = key.find("key") + 3;
        std::string number_str = key.substr(pos);
        int number = std::stoi(number_str);
        array.push_back(number);
    }
    std::sort(array.begin(), array.end());
    // remove duplicates
    array.erase(std::unique(array.begin(), array.end()), array.end());
    for (int i = 1; i <= 20000; i++) {
           std::cout << i << " " << array[i-1] << std::endl;
           EXPECT_TRUE(i == array[i-1]);
    }
    std::remove(rocksdb_path.c_str());
    std::remove(state_db_path.c_str()); 
}

int main() {
    testRocksWalProcessor();
    return 0;
}
