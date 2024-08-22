#pragma once
#include <rocksdb/db.h>
#include <RedisRepo.h>

class RocksWalBatchHandler : public rocksdb::WriteBatch::Handler {
public:
    RocksWalBatchHandler(RedisRepo& redisRepo, std::string queue_name) : redisRepo(redisRepo), queue_name(queue_name) {}
    ~RocksWalBatchHandler() = default;
    rocksdb::Status PutCF(uint32_t column_family_id, const rocksdb::Slice& key, const rocksdb::Slice& value) override;
    rocksdb::Status DeleteCF(uint32_t column_family_id, const rocksdb::Slice& key) override;
private:
    RedisRepo redisRepo;
    std::string queue_name;
};