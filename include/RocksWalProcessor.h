#pragma once

#include <string>
#include <memory>
#include <rocksdb/db.h>
#include <sw/redis++/redis++.h>
#include "RocksWalBatchHandler.h"
#include "RocksRepo.h"
#include "RedisRepo.h"

class RocksWalProcessor {
public:
    RocksWalProcessor(const std::string& redis_host, const std::string& redis_port,
                      const std::string& rocksdb_primary_path, const std::string& rocksdb_state_path,
                      const std::string& rocksdb_walseq_state_key, const std::string& redis_queue_name);
    
    void run();
    void stop();

private:
    std::string redis_host;
    std::string redis_port;
    std::string rocksdb_primary_path;
    std::string rocksdb_state_path;
    std::string rocksdb_walseq_state_key;
    std::string redis_queue_name;
    bool CONTINUE_FLAG = true;
};
