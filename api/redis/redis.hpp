#pragma once

#include <vector>
#include <string>
#include <memory>
#include <hiredis/hiredis.h>

namespace redis {

typedef std::vector<std::string> List;
typedef std::pair<std::string, std::string> KeyPair;

class Redis {
 public:
  Redis(const Redis&) = delete;
  Redis& operator=(const Redis&) = delete;
  Redis();
  ~Redis();

  void LPush(const std::string& key, const std::string& val);
  void RPush(const std::string& key, const std::string& val);
  std::shared_ptr<List> LRange(
    const std::string& key,
    long long start,
    long long end
  );
  std::shared_ptr<List> Sort(
    const std::string& key,
    long long offset,
    long long count
  );
  std::shared_ptr<List> SortBy(
    const std::string& key,
    const std::string& by,
    long long offset,
    long long count
  );
  bool LRem(const std::string& key, const std::string& val);
  bool Del(const std::string& key);
  void Set(const std::string& key, const std::string& val);
  void MSet(std::vector<KeyPair>& vals);
  void HSet(const std::string& key, KeyPair val);
  void HMSet(const std::string& key, std::vector<KeyPair>& vals);
  std::string Get(const std::string& key);
  std::shared_ptr<List> MGet(List& keys);
  std::string HGet(const std::string& key, const std::string& field);
  std::shared_ptr<List> HMGet(const std::string& key, List& fields);
  bool Expire(const std::string& key, long long ttl);
  bool Exists(const std::string& key);

 private:
  redisContext* context_ = nullptr;
};

};