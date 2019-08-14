#include <sstream>
#include <common/api/json/json.hpp>

namespace json {

ExceptionKeyNotFound::ExceptionKeyNotFound(
  const std::string& msg
) : std::runtime_error(msg) {
  msg_ = msg;
}

const char* ExceptionKeyNotFound::what() const throw() {
  return msg_.c_str();
}

ExceptionParseError::ExceptionParseError(
  const std::string& msg
) : std::runtime_error(msg) {
  msg_ = msg;
}

const char* ExceptionParseError::what() const throw() {
  return msg_.c_str();
}

}
