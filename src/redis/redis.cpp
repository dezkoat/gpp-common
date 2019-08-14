#include <sstream>
#include <common/api/redis/redis.hpp>

namespace redis {

Redis::Redis() {
  context_ = redisConnect("localhost", 6379);
}

Redis::~Redis() {
  if (context_ != nullptr) {
    redisFree(context_);
    context_ = nullptr;
  }
}

void Redis::LPush(const std::string& key, const std::string& val) {
  const char method[] = "LPUSH";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(val.c_str());
  argl.push_back(val.size());

  auto v = &(argv[0]);
  auto l = &(argl[0]);

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

void Redis::RPush(const std::string& key, const std::string& val) {
  const char method[] = "RPUSH";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(val.c_str());
  argl.push_back(val.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

bool Redis::LRem(const std::string& key, const std::string& val) {
  const char method[] = "LREM";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back("0");
  argl.push_back(1);

  argv.push_back(val.c_str());
  argl.push_back(val.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  bool result = ((redisReply*)reply)->integer == 1;
  freeReplyObject(reply);

  return result;
}

bool Redis::Del(const std::string& key) {
  const char method[] = "DEL";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  bool result = ((redisReply*)reply)->integer == 1;
  freeReplyObject(reply);

  return result;
}

std::shared_ptr<List> Redis::LRange(
  const std::string& key,
  long long start,
  long long end
) {
  const char method[] = "LRANGE";
  std::vector<const char*> argv;
  std::vector<size_t> argl;
  auto start_s = std::to_string(start);
  auto end_s = std::to_string(end);

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(start_s.c_str());
  argl.push_back(start_s.size());

  argv.push_back(end_s.c_str());
  argl.push_back(end_s.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  auto res = std::make_shared<List>();
  for (int i = 0; i < ((redisReply*)reply)->elements; ++i) {
    res->push_back(((redisReply*)reply)->element[i]->str);
  }
  freeReplyObject(reply);

  return res;
}

std::shared_ptr<List> Redis::Sort(
  const std::string& key,
  long long offset,
  long long count
) {
  const char method[] = "SORT";
  std::vector<const char*> argv;
  std::vector<size_t> argl;
  auto offset_s = std::to_string(offset);
  auto count_s = std::to_string(count);

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  const char params_1[] = "ALPHA";
  argv.push_back(params_1);
  argl.push_back(sizeof(params_1) - 1);

  const char params_2[] = "LIMIT";
  argv.push_back(params_2);
  argl.push_back(sizeof(params_2) - 1);

  argv.push_back(offset_s.c_str());
  argl.push_back(offset_s.size());

  argv.push_back(count_s.c_str());
  argl.push_back(count_s.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  auto res = std::make_shared<List>();
  for (int i = 0; i < ((redisReply*)reply)->elements; ++i) {
    res->push_back(((redisReply*)reply)->element[i]->str);
  }
  freeReplyObject(reply);

  return res;
}

std::shared_ptr<List> Redis::SortBy(
  const std::string& key,
  const std::string& by,
  long long offset,
  long long count
) {
const char method[] = "SORT";
  std::vector<const char*> argv;
  std::vector<size_t> argl;
  auto offset_s = std::to_string(offset);
  auto count_s = std::to_string(count);

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  const char params_1[] = "ALPHA";
  argv.push_back(params_1);
  argl.push_back(sizeof(params_1) - 1);

  const char params_2[] = "BY";
  argv.push_back(params_2);
  argl.push_back(sizeof(params_2) - 1);

  argv.push_back(by.c_str());
  argl.push_back(by.size());

  const char params_3[] = "LIMIT";
  argv.push_back(params_3);
  argl.push_back(sizeof(params_3) - 1);

  argv.push_back(offset_s.c_str());
  argl.push_back(offset_s.size());

  argv.push_back(count_s.c_str());
  argl.push_back(count_s.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  auto res = std::make_shared<List>();
  for (int i = 0; i < ((redisReply*)reply)->elements; ++i) {
    res->push_back(((redisReply*)reply)->element[i]->str);
  }
  freeReplyObject(reply);

  return res;
}

void Redis::Set(const std::string& key, const std::string& val) {
  const char method[] = "SET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(val.c_str());
  argl.push_back(val.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

void Redis::MSet(std::vector<KeyPair>& vals) {
  const char method[] = "MSET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);
  for (auto& val : vals) {
    argv.push_back(val.first.c_str());
    argl.push_back(val.first.size());

    argv.push_back(val.second.c_str());
    argl.push_back(val.second.size());
  }

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

void Redis::HSet(const std::string& key, KeyPair val) {
  const char method[] = "HSET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(val.first.c_str());
  argl.push_back(val.first.size());

  argv.push_back(val.second.c_str());
  argl.push_back(val.second.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

void Redis::HMSet(const std::string& key, std::vector<KeyPair>& vals) {
  const char method[] = "HMSET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  for (auto& val : vals) {
    argv.push_back(val.first.c_str());
    argv.push_back(val.second.c_str());

    argl.push_back(val.first.size());
    argl.push_back(val.second.size());
  }

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  freeReplyObject(reply);
}

std::string Redis::Get(const std::string& key) {
  const char method[] = "GET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  auto result = ((redisReply*)reply)->str;
  freeReplyObject(reply);

  return result;
}

std::shared_ptr<List> Redis::MGet(List& keys) {
  const char method[] = "MGET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  for (auto& val : keys) {
    argv.push_back(val.c_str());
    argl.push_back(val.size());
  }

  auto list = std::make_shared<List>();
  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  for (int i = 0; i < ((redisReply*)reply)->elements; ++i) {
    list->push_back(((redisReply*)reply)->element[i]->str);
  }
  freeReplyObject(reply);

  return list;
}

std::string Redis::HGet(const std::string& key, const std::string& field) {
  const char method[] = "HGET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(field.c_str());
  argl.push_back(field.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  auto result = ((redisReply*)reply)->str;
  freeReplyObject(reply);

  return result;
}

std::shared_ptr<List> Redis::HMGet(
  const std::string& key,
  List& fields
) {
  const char method[] = "HMGET";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  for (auto& val : fields) {
    argv.push_back(val.c_str());
    argl.push_back(val.size());
  }

  auto list = std::make_shared<List>();
  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  for (int i = 0; i < ((redisReply*)reply)->elements; ++i) {
    list->push_back(((redisReply*)reply)->element[i]->str);
  }
  freeReplyObject(reply);

  return list;
}

bool Redis::Expire(const std::string& key, long long ttl) {
  const char method[] = "EXPIRE";
  std::vector<const char*> argv;
  std::vector<size_t> argl;
  auto ttl_s = std::to_string(ttl);

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  argv.push_back(ttl_s.c_str());
  argl.push_back(ttl_s.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  bool result = ((redisReply*)reply)->integer == 1;
  freeReplyObject(reply);

  return result;
}

bool Redis::Exists(const std::string& key) {
  const char method[] = "EXISTS";
  std::vector<const char*> argv;
  std::vector<size_t> argl;

  argv.push_back(method);
  argl.push_back(sizeof(method) - 1);

  argv.push_back(key.c_str());
  argl.push_back(key.size());

  auto reply = redisCommandArgv(context_, argv.size(), &(argv[0]), &(argl[0]));
  bool result = ((redisReply*)reply)->integer == 1;
  freeReplyObject(reply);

  return result;
}

};