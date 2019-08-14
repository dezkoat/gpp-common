#include <random>
#include <sstream>
#include <common/api/util/uuid.hpp>

const char SYMBOLS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

namespace util {

unsigned char RandomChar() {
  std::random_device rd;
  std::mt19937 gen(rd()); 
  std::uniform_int_distribution<> dis(0, sizeof(SYMBOLS) - 2);
  return SYMBOLS[dis(gen)];
}

std::string GenerateUUID() {
  char uuid[33];
  for(auto i = 0; i < 32; i++) {
    uuid[i] = RandomChar();
  }
  uuid[32] = 0;
  return uuid;
}

};