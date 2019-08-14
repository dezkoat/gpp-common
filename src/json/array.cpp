#include <sstream>
#include <common/api/json/json.hpp>

namespace json {

unsigned long Array::GetSize() {
  return elements_.size();
}

std::shared_ptr<Element> Array::Get(unsigned long i) const {
  return elements_[i];
}

bool Array::PutString(const std::string& val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutInteger(long long val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutDouble(long double val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutBoolean(bool val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutArray(std::shared_ptr<Array> val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutObject(std::shared_ptr<Object> val) {
  return PutValue(std::make_shared<Element>(val));
}

bool Array::PutValue(std::shared_ptr<Element> val) {
  elements_.push_back(val);

  return true;
}

inline void replace_(
  std::string& str,
  const std::string& substr,
  const std::string& rep
) {
  size_t index = 0;
  while (true) {
    index = str.find(substr, index);
    if (index == std::string::npos) {
      break;
    }

    str.replace(index, 2, rep);
    index += 2;
  }
}

std::string Array::ToString() const {
  std::stringstream ss;

  ss << '[';
  bool first = true;
  for (auto it : elements_) {
    if (first) {
      first = false;
    } else {
      ss << ',';
    }

    switch (it->GetType()) {
    case Type::STRING:
      {
        // sanitize string
        std::string str = it->AsString();
        replace_(str, "\"", "\\\"");
        replace_(str, "\n", "\\n");

        ss << '"' << str << '"';
      }
      break;
    case Type::ARRAY:
      ss << it->AsArray()->ToString();
      break;
    case Type::BOOLEAN:
      ss << (it->AsBoolean() ? "true" : "false");
      break;
    case Type::DOUBLE:
      ss << it->AsDouble();
      break;
    case Type::INTEGER:
      ss << it->AsInteger();
      break;
    case Type::OBJECT:
      ss << it->AsObject()->ToString();
      break;
    default:
      throw ExceptionParseError("Invalid Type");
    }
  }
  ss << ']';

  return ss.str();
}


};