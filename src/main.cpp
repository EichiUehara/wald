#include <iostream>
#include <rocksdb/db.h>
#include "RocksWalProcessor.h"


int main() {
    std::string redis_host = std::getenv("REDIS_HOST");
    std::string redis_port = std::getenv("REDIS_PORT");
    std::string rocksdb_primary_path = std::getenv("ROCKSDB_PRIMARY_PATH");
    std::string rocksdb_state_path = std::getenv("ROCKSDB_STATE_PATH");
    std::string rocksdb_walseq_state_key = std::getenv("ROCKSDB_WALSEQ_STATE_KEY");
    std::string redis_queue_name = std::getenv("REDIS_QUEUE_NAME");

    try {
        RocksWalProcessor processor(redis_host, redis_port, rocksdb_primary_path, rocksdb_state_path,
                                     rocksdb_walseq_state_key, redis_queue_name);
        processor.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
