#include <common/api/json/parser.hpp>

namespace json {

enum ParserState {
  FETCH_KEY,
  FETCH_KEY_AFTER_COMMA,
  FETCH_COLON_SEPARATOR,
  FETCH_VALUE,
  FETCH_COMMA,
  INSIDE_STRING,
};
  
std::shared_ptr<Object> Parser::ParseObject(const std::string& str, int& p) {
  std::shared_ptr<Object> obj(std::make_shared<Object>());
  if (str[p] == 'n') {
    if (str[++p] != 'u') throw ExceptionParseError("Invalid JSON string");
    if (str[++p] != 'l') throw ExceptionParseError("Invalid JSON string");
    if (str[++p] != 'l') throw ExceptionParseError("Invalid JSON string");
    return obj;
  }

  if (str[p] != '{') {
    throw ExceptionParseError(std::string("Invalid JSON string: [") + str[p] + "]");
  }

  std::string key;
  ParserState state = FETCH_KEY;
  while (++p < str.length()) {
    if (state != INSIDE_STRING) {
      if (str[p] == ' ' || str[p] == '\n' || str[p] == '\t') {
        continue;
      }
    }

    if (state <= FETCH_KEY_AFTER_COMMA) {
      if (str[p] == '"') {
        state = INSIDE_STRING;
      } else if (str[p] == '}') {
        if (state == FETCH_KEY_AFTER_COMMA) {
          throw ExceptionParseError("Trailing comma in object");
        }

        break;
      } else {
        throw ExceptionParseError("Expecting key");
      }
    } else if (state == INSIDE_STRING) {
      if (str[p] == '"') {
        state = FETCH_COLON_SEPARATOR;
      } else {
        if (str[p] == '\\') {
          ++p;
          if (str[p] == 'n') key += '\n';
          if (str[p] == 'r') key += '\r';
          if (str[p] == 't') key += '\t';
          if (str[p] == '"') key += '\"';
        } else {
          key += str[p];
        }
      }
    } else if (state == FETCH_COLON_SEPARATOR) {
      if (str[p] == ':')
        state = FETCH_VALUE;
    } else if (state == FETCH_VALUE) {
      std::shared_ptr<Element> val = ParseValue(str, p);
      obj->PutValue(key, val);

      key = "";
      state = FETCH_COMMA;
    } else if (state == FETCH_COMMA) {
      if (str[p] == ',') {
        state = FETCH_KEY_AFTER_COMMA;
      } else if (str[p] == '}') {
        break;
      } else {
        throw ExceptionParseError("Expecting comma or closing curly brace");
      }
    }
  }

  return obj;
}

std::shared_ptr<Element> Parser::ParseValue(const std::string& str, int& p) {
  if (str[p] == '"') {
    std::string val = Parser::ParseString(str, p);
    return std::shared_ptr<Element>(std::make_shared<Element>(val));
  } else if (str[p] == '{' || str[p] == 'n') {
    std::shared_ptr<Object> val = Parser::ParseObject(str, p);
    return std::shared_ptr<Element>(std::make_shared<Element>(val));
  } else if (str[p] == '-' || str[p] == '+'
      || (str[p] >= '0' && str[p] <= '9')) {
    return Parser::ParseNumber(str, p);
  } else if (str[p] == 't' || str[p] == 'f') {
    bool val = Parser::ParseBoolean(str, p);
    return std::shared_ptr<Element>(std::make_shared<Element>(val));
  } else if (str[p] == '[') {
    std::shared_ptr<Array> val = Parser::ParseArray(str, p);
    return std::shared_ptr<Element>(std::make_shared<Element>(val));
  }

  throw ExceptionParseError("Error while parsing a value");
}

std::string Parser::ParseString(const std::string& str, int& p) {
  std::string val;
  while (++p < str.length()) {
    if (str[p] == '"') {
      return val;
    }

    if (str[p] == '\\') {
      ++p;
      if (str[p] == 'n') val += '\n';
      if (str[p] == 'r') val += '\r';
      if (str[p] == 't') val += '\t';
      if (str[p] == '"') val += '\"';
    } else {
      val += str[p];
    }
  }

  return val;
}

bool Parser::ParseBoolean(const std::string& str, int& p) {
  if (str[p+1] == 'r' && str[p+2] == 'u' && str[p+3] == 'e') {
    p += 3;
    return true;
  }

  if (str[p+1] == 'a' && str[p+2] == 'l' && str[p+3] == 's'
      && str[p+4] == 'e') {
    p += 4;
    return false;
  }

  throw ExceptionParseError("Error while parsing a boolean");
}

std::shared_ptr<Element> Parser::ParseNumber(const std::string& str, int& p) {
  bool is_double = false;
  int start = p;
  while (true) {
    if (str[p] == ',' || str[p] == ' ' || str[p] == '}') {
      break;
    }

    if (str[p] == '.') {
      is_double = true;
    }

    ++p;
  }

  std::string val = str.substr(start, p - start);
  --p;

  if (is_double) {
    return std::shared_ptr<Element>(std::make_shared<Element>(stold(val)));
  }

  return std::shared_ptr<Element>(std::make_shared<Element>(stoll(val)));
}

std::shared_ptr<Array> Parser::ParseArray(const std::string& str, int& p) {
  std::shared_ptr<Array> arr(std::make_shared<Array>());

  ++p;
  while (true) {
    if (str[p] == ']') {
      if (arr->GetSize() > 0) {
        throw ExceptionParseError("Trailing comma in array");
      }

      return arr; // Empty array
    }

    std::shared_ptr<Element> val = ParseValue(str, p);
    arr->PutValue(val);

    while (++p < str.length()) {
      if (str[p] == ',') {
        ++p;
        break;
      }

      if (str[p] == ']') {
        return arr;
      }

      if (str[p] == ' ' || str[p] == '\n' || str[p] == '\r' || str[p] == '\t') {
        continue;
      }

      throw ExceptionParseError("Expecting comma or closing square bracket");
    }

    if (str[p] == ']') {
      break;
    }

    if (p >= str.length()) {
      throw ExceptionParseError("Expecting comma or closing square bracket");
    }
  }

  throw ExceptionParseError("Error while parsing an array");
}

}