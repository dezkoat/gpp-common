#pragma once

#include <memory>
#include <string>
#include <common/api/json/json.hpp>

namespace json {

class Parser {
 public:
  static std::shared_ptr<Object> ParseObject(const std::string&, int&);
  static std::shared_ptr<Element> ParseValue(const std::string&, int&);
  static std::string ParseString(const std::string&, int&);
  static bool ParseBoolean(const std::string&, int&);
  static bool ParseNull(const std::string&, int&);
  static std::shared_ptr<Element> ParseNumber(const std::string&, int&);
  static std::shared_ptr<Array> ParseArray(const std::string&, int&);
};

};