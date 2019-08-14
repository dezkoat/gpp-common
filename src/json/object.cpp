#include <sstream>
#include <common/api/json/json.hpp>
#include <common/api/json/parser.hpp>

namespace json {

Object::Object() : null_(true), elements_(std::make_shared<Map>()) {}

Object::Object(const std::string& json)
  : null_(true), elements_(std::make_shared<Map>())
{
  int p = 0;
  auto obj = Parser::ParseObject(json, p);
  elements_ = obj->elements_;
  null_ = obj->null_;
}

std::string Object::GetString(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }

  return it->second->AsString();
}

long long Object::GetInteger(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }

  return it->second->AsInteger();
}

long double Object::GetDouble(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }
  
  return it->second->AsDouble();
}

bool Object::GetBoolean(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }
  
  return it->second->AsBoolean();
}

std::shared_ptr<Array> Object::GetArray(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }
  
  return it->second->AsArray();
}

std::shared_ptr<Object> Object::GetObject(const std::string& key) const {
  auto it = elements_->find(key);
  if (it == elements_->end()) {
    throw ExceptionKeyNotFound("Key " + key + " not found");
  }
  
  return it->second->AsObject();
}

bool Object::IsNull() const {
  return null_;
}

bool Object::PutValue(const std::string& key, std::shared_ptr<Element> val) {
  null_ = false;
  elements_->insert(PairKey(key, val));

  return true;
}

bool Object::PutString(const std::string& key, const std::string& val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::PutInteger(const std::string& key, long long val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::PutDouble(const std::string& key, long double val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::PutBoolean(const std::string& key, bool val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::PutArray(const std::string& key, std::shared_ptr<Array> val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::PutObject(const std::string& key, std::shared_ptr<Object> val) {
  return PutValue(key, std::make_shared<Element>(val));
}

bool Object::DeleteKey(const std::string& key) {
  elements_->erase(key);

  return true;
}

bool Object::SetNull(bool val) {
  null_ = val;
  if (null_ == true)
    elements_->clear();
  
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

    str.replace(index, 1, rep);
    index += 2;
  }
}

std::string Object::ToString() const {
  std::stringstream ss;

  if (null_) {
    return "null";
  }

  ss << '{';
  bool first = true;
  for (auto it : *elements_) {
    if (first) {
      first = false;
    } else {
      ss << ',';
    }

    ss << '"' << it.first << "\":";
    switch (it.second->GetType()) {
    case Type::STRING:
      {
        // sanitize string
        std::string str = it.second->AsString();
        replace_(str, "\"", "\\\"");
        replace_(str, "\n", "\\n");

        ss << '"' << str << '"';
      }
      break;
    case Type::ARRAY:
      ss << it.second->AsArray()->ToString();
      break;
    case Type::BOOLEAN:
      ss << (it.second->AsBoolean() ? "true" : "false");
      break;
    case Type::DOUBLE:
      ss << it.second->AsDouble();
      break;
    case Type::INTEGER:
      ss << it.second->AsInteger();
      break;
    case Type::OBJECT:
      ss << it.second->AsObject()->ToString();
      break;
    default:
      throw ExceptionParseError("Invalid Type");
    }
  }
  ss << '}';

  return ss.str();
}


};