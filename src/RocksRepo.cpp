#include <rocksdb/db.h>
#include "RocksRepo.h"

void RocksRepo::put(std::string key, std::string value) {
    auto status = db->Put(rocksdb::WriteOptions(), key, value);
    if (!status.ok()) {
    }
}

std::string RocksRepo::get(std::string key) {
    std::string value;
    auto status = db->Get(rocksdb::ReadOptions(), key, &value);
    if (!status.ok()) {
        value = "";
    }
    return value;
}

void RocksRepo::del(std::string key) {
    auto status = db->Delete(rocksdb::WriteOptions(), key);
    if (!status.ok()) {
    }
}