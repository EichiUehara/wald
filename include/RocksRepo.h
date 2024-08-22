#pragma once
#include <rocksdb/db.h>

class RocksRepo {
public:
    RocksRepo(rocksdb::DB* db) : db(db) {}
    ~RocksRepo() = default;
    void put(std::string key, std::string value);
    std::string get(std::string key);
    void del(std::string key);
private:
    rocksdb::DB* db;
};
