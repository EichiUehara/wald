#include "RocksWalProcessor.h"
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iostream>

RocksWalProcessor::RocksWalProcessor(const std::string& redis_host, const std::string& redis_port,
                                       const std::string& rocksdb_primary_path, const std::string& rocksdb_state_path,
                                       const std::string& rocksdb_walseq_state_key, const std::string& redis_queue_name)
    : redis_host(redis_host), redis_port(redis_port), rocksdb_primary_path(rocksdb_primary_path),
      rocksdb_state_path(rocksdb_state_path), rocksdb_walseq_state_key(rocksdb_walseq_state_key),
      redis_queue_name(redis_queue_name) {}

void RocksWalProcessor::run() {
    while(true) {
        try{
            std::string redis_url = "tcp://" + redis_host + ":" + redis_port;
            auto redis_con = sw::redis::Redis(redis_url);
            rocksdb::DB* db;
            rocksdb::DB* state_db;

            auto options = rocksdb::Options();
            options.create_if_missing = true;
            options.WAL_ttl_seconds = 10;
            options.WAL_size_limit_MB = 100;

            auto status = rocksdb::DB::OpenForReadOnly(options, rocksdb_primary_path, &db);
            if (!status.ok()) {
                throw std::runtime_error("Failed to open DB: " + status.ToString());
            }

            status = rocksdb::DB::Open(options, rocksdb_state_path, &state_db);
            if (!status.ok()) {
                throw std::runtime_error("Failed to open DB: " + status.ToString());
            }

            auto redis_repo = RedisRepo(redis_con);
            auto state_rocks = RocksRepo(state_db);
 
            // TODO: Implement store for already read sequence for each WAL log file
            // {filename: sequence, ...}
            RocksWalBatchHandler handler(redis_repo, redis_queue_name);
            std::unique_ptr<rocksdb::TransactionLogIterator> iter;
            auto rocksdb_walseq_state_value = state_rocks.get(rocksdb_walseq_state_key);
            if (rocksdb_walseq_state_value.empty()) {
                state_rocks.put(rocksdb_walseq_state_key, "0");
            }
            auto num_seq_start = std::stoi(state_rocks.get(rocksdb_walseq_state_key));
            status = db->GetUpdatesSince(num_seq_start, &iter);
            if (!status.ok()) {
                status = db->GetUpdatesSince(0, &iter);
                num_seq_start = 0;
            }
            while (true) {
                if(this->CONTINUE_FLAG == true){
                    if (iter->Valid()) {
                        auto batch_result = iter->GetBatch();
                        if (batch_result.writeBatchPtr) {
                            batch_result.writeBatchPtr->Iterate(&handler);
                        }
                        auto num_seq_read = batch_result.sequence;
                        num_seq_start += num_seq_read;
                        state_rocks.put(rocksdb_walseq_state_key, std::to_string(num_seq_start));
                        iter->Next();
                    } else {
                        delete db;
                        delete state_db;
                        throw std::runtime_error("Iterator is not valid");
                    }
                } else {
                    delete db;
                    delete state_db;
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
    }        
}

void RocksWalProcessor::stop() {
    this->CONTINUE_FLAG = false;
}