#include <common/api/json/json.hpp>

namespace json {

Element::Element(const std::string& val) {
  SetString(val);
}

Element::Element(long long val) {
  SetInteger(val);
}

Element::Element(long double val) {
  SetDouble(val);
}

Element::Element(bool val) {
  SetBoolean(val);
}

Element::Element(std::shared_ptr<Array> val) {
  SetArray(val);
}

Element::Element(std::shared_ptr<Object> val) {
  SetObject(val);
}

void Element::SetString(const std::string& val) {
  str_ = val;
  type_ = Type::STRING;
}

void Element::SetInteger(long long val) {
  int_ = val;
  type_ = Type::INTEGER;
}

void Element::SetDouble(long double val) {
  double_ = val;
  type_ = Type::DOUBLE;
}

void Element::SetBoolean(bool val) {
  bool_ = val;
  type_ = Type::BOOLEAN;
}

void Element::SetArray(std::shared_ptr<Array> val) {
  array_ = val;
  type_ = Type::ARRAY;
}

void Element::SetObject(std::shared_ptr<Object> val) {
  object_ = val;
  type_ = Type::OBJECT;
}

std::string Element::AsString() const {
  return str_;
}

long long Element::AsInteger() const {
  return int_;
}

long double Element::AsDouble() const {
  return double_;
}

bool Element::AsBoolean() const {
  return bool_;
}

std::shared_ptr<Array> Element::AsArray() const {
  return array_;
}

std::shared_ptr<Object> Element::AsObject() const {
  return object_;
}

Type Element::GetType() const {
  return type_;
}

};