#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace json {

enum Type {
  STRING,
  INTEGER,
  DOUBLE,
  BOOLEAN,
  ARRAY,
  OBJECT
};

class Array;
class Object;

class Element {
 public:
  Element(const std::string&);
  Element(long long);
  Element(long double);
  Element(bool);
  Element(std::shared_ptr<Array>);
  Element(std::shared_ptr<Object>);

  std::string AsString() const;
  long long AsInteger() const;
  long double AsDouble() const;
  bool AsBoolean() const;
  std::shared_ptr<Array> AsArray() const;
  std::shared_ptr<Object> AsObject() const;

  void SetString(const std::string&);
  void SetInteger(long long);
  void SetDouble(long double);
  void SetBoolean(bool);
  void SetArray(std::shared_ptr<Array>);
  void SetObject(std::shared_ptr<Object>);
  
  Type GetType() const;

 private:
  Type type_;
  union {
    long long int_;
    long double double_;
    bool bool_;
  };
  std::string str_;
  std::shared_ptr<Array> array_;
  std::shared_ptr<Object> object_;
};

typedef std::pair<std::string, std::shared_ptr<Element>> PairKey;
typedef std::map<std::string, std::shared_ptr<Element>> Map;

class Array {
 public:
  unsigned long GetSize();
  std::shared_ptr<Element> Get(unsigned long i) const;

  bool PutString(const std::string&);
  bool PutInteger(long long);
  bool PutDouble(long double);
  bool PutBoolean(bool);
  bool PutArray(std::shared_ptr<Array>);
  bool PutObject(std::shared_ptr<Object>);
  bool PutValue(std::shared_ptr<Element>);

  std::string ToString() const;

 private:
  std::vector<std::shared_ptr<Element>> elements_;

  friend class Object;
};

class Object {
 public:
  Object();
  Object(const std::string&);

  std::string GetString(const std::string&) const;
  long long GetInteger(const std::string&) const;
  long double GetDouble(const std::string&) const;
  bool GetBoolean(const std::string&) const;
  std::shared_ptr<Array> GetArray(const std::string&) const;
  std::shared_ptr<Object> GetObject(const std::string&) const;
  bool IsNull() const;
  bool SetNull(bool);

  bool PutString(const std::string&, const std::string&);
  bool PutInteger(const std::string&, long long);
  bool PutDouble(const std::string&, long double);
  bool PutBoolean(const std::string&, bool);
  bool PutArray(const std::string&, std::shared_ptr<Array>);
  bool PutObject(const std::string&, std::shared_ptr<Object>);
  bool PutValue(const std::string&, std::shared_ptr<Element>);
  bool DeleteKey(const std::string&);

  std::string ToString() const;

 private:
  std::shared_ptr<Map> elements_;
  bool null_;

  friend class Parser;
};

class ExceptionKeyNotFound : public std::runtime_error {
 public:
  ExceptionKeyNotFound(const std::string& msg);
  ~ExceptionKeyNotFound() throw() {};
  const char* what() const throw();

 private:
  std::string msg_;
};

class ExceptionParseError : public std::runtime_error {
 public:
  ExceptionParseError(const std::string& msg);
  ~ExceptionParseError() throw() {};
  const char* what() const throw();

 private:
  std::string msg_;
};

};