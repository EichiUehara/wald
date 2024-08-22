#include <iostream>
#include "RocksWalBatchHandler.h"
#include <RedisRepo.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

rocksdb::Status RocksWalBatchHandler::PutCF(
    uint32_t column_family_id, 
    const rocksdb::Slice &key, const rocksdb::Slice &value)
{
    try{
        json j;
        auto unix_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        j["t"] = unix_timestamp;
        j["k"] = key.ToString();
        j["v"] = value.ToString();
        j["op"] = "Put";
        std::string jsonStr = j.dump();
        redisRepo.SendToRedisQueue(queue_name, jsonStr);
        #ifdef DEBUG
        // std::cout << "PutCF: " << jsonStr << std::endl;
        #endif
        return rocksdb::Status::OK();
    }catch(const std::exception& e){
        std::cout << "Error: " << e.what() << std::endl;
        return rocksdb::Status::Corruption("Error in PutCF");
    }
}

rocksdb::Status RocksWalBatchHandler::DeleteCF(uint32_t column_family_id, const rocksdb::Slice &key)
{
    try{
        json j;
        j["t"] = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        j["k"] = key.ToString();
        j["op"] = "Delete";
        std::string jsonStr = j.dump();
        redisRepo.SendToRedisQueue(queue_name, jsonStr);
        #ifdef DEBUG
        std::cout << "DeleteCF: " << jsonStr << std::endl;
        #endif
        return rocksdb::Status::OK();
    }catch(const std::exception& e){
        std::cout << "Error: " << e.what() << std::endl;
        return rocksdb::Status::Corruption("Error in DeleteCF");
    }
}