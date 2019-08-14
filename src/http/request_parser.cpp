#include <algorithm>
#include <sstream>
#include <common/api/http/request.hpp>
#include <common/api/http/request_parser.hpp>

namespace http {

RequestParser::RequestParser() : state_(METHOD_START) {
}

#define SETRET(state) { state_ = state; return INDETERMINATE; }
#define SETBAD() return BAD;
RequestParser::ResultType RequestParser::ConsumeHeader(
  Request& req,
  char input
) {
  switch (state_) {
  case METHOD_START:
    if (!IsChar(input) || IsCtl(input) || IsTspecial(input))
      SETBAD();
    method_ = input;
    SETRET(METHOD);
  case METHOD:
    if (input == ' ') {
      if (method_.compare("POST") == 0)
        req.method_ = Request::MethodType::POST;
      else if (method_.compare("PUT") == 0)
        req.method_ = Request::MethodType::PUT;
      else if (method_.compare("DELETE") == 0)
        req.method_ = Request::MethodType::DELETE;
      else
        req.method_ = Request::MethodType::GET;
      SETRET(URI);
    }
    if (!IsChar(input) || IsCtl(input) || IsTspecial(input))
      SETBAD();
    method_.push_back(input);
    SETRET(METHOD);
  case URI:
    if (input == ' ') {
      RequestParser::UrlDecode(std::string(req.uri_), req.uri_);
      if (req.uri_.empty()
          || req.uri_[0] != '/'
          || req.uri_.find("..") != std::string::npos)
        SETBAD();

      SETRET(HTTP_VERSION_H);
    }
    if (IsCtl(input))
      SETBAD();
    req.uri_.push_back(input);
    SETRET(URI);
  case HTTP_VERSION_H:
    if (input == 'H')
      SETRET(HTTP_VERSION_T_1);
    SETBAD();
  case HTTP_VERSION_T_1:
    if (input == 'T')
      SETRET(HTTP_VERSION_T_2);
    SETBAD();
  case HTTP_VERSION_T_2:
    if (input == 'T')
      SETRET(HTTP_VERSION_P);
    SETBAD();
  case HTTP_VERSION_P:
    if (input == 'P')
      SETRET(HTTP_VERSION_SLASH);
    SETBAD();
  case HTTP_VERSION_SLASH:
    if (input == '/') {
      req.http_version_major_ = 0;
      req.http_version_minor_ = 0;
      SETRET(HTTP_VERSION_MAJOR_START);
    }
    SETBAD();
  case HTTP_VERSION_MAJOR_START:
    if (IsDigit(input)) {
      req.http_version_major_ = input - '0';
      SETRET(HTTP_VERSION_MAJOR);
    }
    SETBAD();
  case HTTP_VERSION_MAJOR:
    if (input == '.')
      SETRET(HTTP_VERSION_MINOR_START);
    if (IsDigit(input)) {
      req.http_version_major_ = req.http_version_major_ * 10 + input - '0';
      SETRET(HTTP_VERSION_MAJOR);
    }
    SETBAD();
  case HTTP_VERSION_MINOR_START:
    if (IsDigit(input)) {
      req.http_version_minor_ = input - '0';
      SETRET(HTTP_VERSION_MINOR);
    }
    SETBAD();
  case HTTP_VERSION_MINOR:
    if (input == '\r')
      SETRET(EXPECTING_NEWLINE_1);
    if (IsDigit(input)) {
      req.http_version_minor_ = req.http_version_minor_ * 10 + input - '0';
      SETRET(HTTP_VERSION_MINOR);
    }
    SETBAD();
  case EXPECTING_NEWLINE_1:
    if (input == '\n')
      SETRET(HEADER_LINE_START);
    SETBAD();
  case HEADER_LINE_START:
    if (input == '\r')
      SETRET(EXPECTING_NEWLINE_3);
    if (!req.headers_.empty() && (input == ' ' || input == '\t'))
      SETRET(HEADER_LWS);
    if (!IsChar(input) || IsCtl(input) || IsTspecial(input))
      SETBAD();
    if (!header_key_.empty())
      req.headers_.insert(SSPair(header_key_, header_val_));
    header_key_ = tolower(input);
    header_val_.clear();
    SETRET(HEADER_NAME);
  case HEADER_LWS:
    if (input == '\r')
      SETRET(EXPECTING_NEWLINE_2);
    if (input == ' ' || input == '\t')
      SETRET(HEADER_LWS);
    if (IsCtl(input))
      SETBAD();
    header_val_.push_back(input);
    SETRET(HEADER_VALUE);
  case HEADER_NAME:
    if (input == ':')
      SETRET(SPACE_BEFORE_HEADER_VALUE);
    if (!IsChar(input) || IsCtl(input) || IsTspecial(input))
      SETBAD();
    header_key_.push_back(tolower(input));
    SETRET(HEADER_NAME);
  case SPACE_BEFORE_HEADER_VALUE:
    if (input == ' ')
      SETRET(HEADER_VALUE);
    SETBAD();
  case HEADER_VALUE:
    if (input == '\r')
      SETRET(EXPECTING_NEWLINE_2);
    if (IsCtl(input))
      SETBAD();
    header_val_.push_back(input);
    SETRET(HEADER_VALUE);
  case EXPECTING_NEWLINE_2:
    if (input == '\n')
      SETRET(HEADER_LINE_START);
    SETBAD();
  case EXPECTING_NEWLINE_3:
    if (!header_key_.empty())
      req.headers_.insert(SSPair(header_key_, header_val_));
    state_ = CONTENT_BODY;
    return input == '\n' ? GOOD : BAD;
  default:
    SETBAD();
  }
}

RequestParser::ResultType RequestParser::ConsumePayload(
  Request& req,
  char* begin
) {
  auto content_length_s = req.headers_.find("content-length");
  if (content_length_s != req.headers_.end()) {
    auto content_length = std::stol(content_length_s->second);
    req.body_ += begin;
    if (req.body_.length() < content_length)
      return INDETERMINATE;
    else if (req.body_.length() > content_length)
      return BAD;
  }

  return GOOD;
}

bool RequestParser::IsChar(int c) {
  return c >= 10 && c <= 127;
}

bool RequestParser::IsCtl(int c) {
  return (c >= 0 && c <= 31) || c == 127;
}

bool RequestParser::IsTspecial(int c) {
  switch (c) {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool RequestParser::IsDigit(int c) {
  return c >= '0' && c <= '9';
}

bool RequestParser::UrlDecode(const std::string& in, std::string& out) {
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else
          return false;
      } else
        return false;
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }

  return true;
}

};