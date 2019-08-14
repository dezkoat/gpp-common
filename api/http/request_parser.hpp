#pragma once

#include <string>
#include <iostream>
#include <tuple>
#include <common/api/http/request.hpp>

namespace http {

class RequestParser {
 public:
  enum ResultType { GOOD, BAD, INDETERMINATE };

  RequestParser();

  RequestParser::ResultType Parse(
    Request& req,
    char* begin,
    char* end
  ) {
    if (state_ == CONTENT_BODY)
      return ConsumePayload(req, begin);

    while (begin != end) {
      ResultType result = ConsumeHeader(req, *begin++);
      if (result == GOOD || result == BAD) {
        return ConsumePayload(req, begin);
      }
    }

    return INDETERMINATE;
  }

 private:
  ResultType ConsumeHeader(Request& req, char input);
  ResultType ConsumePayload(Request& req, char* begin);

  std::string method_;
  std::string header_key_;
  std::string header_val_;

  static bool IsChar(int c);
  static bool IsCtl(int c);
  static bool IsTspecial(int c);
  static bool IsDigit(int c);

  static bool UrlDecode(const std::string& in, std::string& out);

  enum State {
    METHOD_START,
    METHOD,
    URI,
    HTTP_VERSION_H,
    HTTP_VERSION_T_1,
    HTTP_VERSION_T_2,
    HTTP_VERSION_P,
    HTTP_VERSION_SLASH,
    HTTP_VERSION_MAJOR_START,
    HTTP_VERSION_MAJOR,
    HTTP_VERSION_MINOR_START,
    HTTP_VERSION_MINOR,
    EXPECTING_NEWLINE_1,
    HEADER_LINE_START,
    HEADER_LWS,
    HEADER_NAME,
    SPACE_BEFORE_HEADER_VALUE,
    HEADER_VALUE,
    EXPECTING_NEWLINE_2,
    EXPECTING_NEWLINE_3,
    CONTENT_BODY,
  } state_;
};

};