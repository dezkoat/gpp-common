#include <iostream>
#include <common/api/json/json.hpp>

int main() {
  // READ JSON
  std::string json = "{\"test\":\"HELLOWORLD\",\"bool\":true,\"fls\":false,\"nul\":null,\"int\":42,\"flot\":42.04,\"min\":-5,\"flotmin\":-5.34,\"arr\":[\"sdasd\",3424,-34,23.6,[]],\"obj\":{\"inside\":\"im inside\",\"gc\":{\"hello\":\"im grandchld\"}}}";

  json::Object obj(json);
  std::cout << obj.GetString("test") << std::endl;
  std::cout << obj.GetBoolean("bool") << std::endl;
  std::cout << obj.GetBoolean("fls") << std::endl;
  std::cout << obj.GetInteger("int") << std::endl;
  std::cout << obj.GetDouble("flot") << std::endl;
  std::cout << obj.GetInteger("min") << std::endl;
  std::cout << obj.GetDouble("flotmin") << std::endl;
  if (obj.GetObject("nul")->IsNull()) {
    std::cout << "null" << std::endl;
  }

  std::shared_ptr<json::Array> arr = obj.GetArray("arr");
  std::cout << "Array size: " << arr->GetSize() << std::endl;
  for (int i = 0; i < arr->GetSize(); ++i) {
    std::shared_ptr<json::Element> el = arr->Get(i);
    switch (el->GetType()) {
    case json::Type::INTEGER:
      std::cout << "Integer: " << el->AsInteger() << std::endl;
      break;
    case json::Type::DOUBLE:
      std::cout << "Double: " << el->AsDouble() << std::endl;
      break;
    case json::Type::STRING:
      std::cout << "String: " << el->AsString() << std::endl;
      break;
    case json::Type::BOOLEAN:
      std::cout << "Boolean: " << el->AsBoolean() << std::endl;
      break;
    case json::Type::OBJECT:
      std::cout << "me object" << std::endl;
      break;
    case json::Type::ARRAY:
      std::cout << "me array" << std::endl;
      break;
    default:
      break;
    }
  }

  std::cout << obj.GetObject("obj")->GetObject("gc")->GetString("hello") << std::endl;

  // Write JSON
  json::Object obj2;
  obj2.PutString("me", "likey");
  obj2.PutInteger("num", 123);
  obj2.PutDouble("flo", 123.123);
  obj2.PutInteger("num2", -456);
  obj2.PutDouble("flo2", -456.456);
  obj2.PutBoolean("bol", false);
  obj2.PutBoolean("bol2", true);

  json::Object nul;
  obj2.PutObject("nul", std::make_shared<json::Object>(nul));

  json::Object child;
  json::Object gc;

  gc.PutString("me", "grandchild");
  child.PutObject("grchild", std::make_shared<json::Object>(gc));
  obj2.PutObject("child", std::make_shared<json::Object>(child));
  
  json::Array arrx;
  json::Array arrempty;
  arrx.PutArray(std::make_shared<json::Array>(arrempty));
  arrx.PutInteger(19);
  arrx.PutDouble(20.0);
  arrx.PutObject(std::make_shared<json::Object>(nul));
  obj2.PutArray("arr", std::make_shared<json::Array>(arrx));

  std::cout << "Result: ";
  std::cout << obj2.ToString() << std::endl;
  return 0;
}