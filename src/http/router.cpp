#include <functional>
#include <common/api/http/router.hpp>

namespace http {

void Router::SetRoute(std::string route) {
  route_ = route;
}

Response::StatusType ParsePathVariables(
  const std::string& uri,
  const std::string& endpoint,
  std::map<std::string, std::string>& path_variables
) {
  path_variables.clear();
  int a = 0;
  int b = 0;
  int c = 1024; // MAX LENGTH FOR URI
  std::string key, value;
  while (--c > 0) {
    if (endpoint[b] == '{') {
      key = value = "";
      while (endpoint[++b] != '}') {
        key += endpoint[b];
      }
      ++b;
      while (uri[a] != 0 && uri[a] != '/') {
        value += uri[a++];
      }
      path_variables.insert(std::pair<std::string, std::string>(key, value));
    }


    if (uri[a] != endpoint[b]) {
      return Response::NOT_FOUND;
    } else if (uri[a] == endpoint[b] && uri[a] == 0) {
      return Response::OK;
    }

    if (uri[++a] == 0 || endpoint[++b] == 0)
      return Response::NOT_FOUND;
  }

  return Response::NOT_FOUND;
}

std::map<std::string, std::string> ParseParameters(std::string& uri) {
  std::map<std::string, std::string> parameters;
  std::string::size_type separator = uri.find('?');
  if (separator == -1) {
    return parameters;
  }

  std::string params = uri.substr(separator + 1, uri.length() - separator - 1);

  std::string key, val;
  bool parsing_key = true;
  for (int i = 0; i < params.length(); ++i) {
    if (parsing_key) {
      if (params[i] == '=') {
        parsing_key = false;
      } else if (params[i] == '&') {
        if (!key.empty()) {
          parameters.insert(SSPair(key, "true"));
          key.clear();
        }
      } else {
        key.push_back(params[i]);
      }
    } else {
      if (params[i] == '&') {
        parameters.insert(SSPair(key, val));
        key.clear();
        val.clear();
        parsing_key = true;
      } else {
        val.push_back(params[i]);
      }
    }
  }

  if (!key.empty()) {
    if (val.empty()) {
      parameters.insert(SSPair(key, "true"));
    } else {
      parameters.insert(SSPair(key, val));
    }
  }

  uri.erase(separator, uri.length() - separator);

  return parameters;
}

void Router::HandleRequest(Request& req, Response& res) {
  std::string uri_relative(req.uri_);
  uri_relative.erase(0, route_.length());

  if (!uri_relative.empty()) {
    req.parameters_ = ParseParameters(uri_relative);
  }

  // Search for static endpoints first
  auto it = endpoints_static_.find(PairKey(uri_relative, req.GetMethod()));
  if (it != endpoints_static_.end()) {
    std::bind(it->second, this, std::ref(req), std::ref(res))();
    return;
  }

  // Then search for dynamic endpoints second
  for (auto it : endpoints_dynamic_) {
    if (it.first.second != req.GetMethod())
      continue;

    Response::StatusType status = ParsePathVariables(
      uri_relative,
      it.first.first,
      req.path_variables_
    );

    if (status == Response::OK) {
      std::bind(it.second, this, std::ref(req), std::ref(res))();
      return;
    }
  }

  res = Response::Generate(Response::NOT_FOUND);
}

void Router::AddEndpoint(
  std::string endpoint,
  Request::MethodType method_type,
  RouterMethod method
) {
  if (endpoint[endpoint.size() - 1] == '/')
    endpoint.pop_back();

  if (endpoint.find('{') != -1) {
    endpoints_dynamic_.push_back(
      PairMethod(PairKey(endpoint, method_type), method)
    );
  } else {
    endpoints_static_.insert(
      PairMethod(PairKey(endpoint, method_type), method)
    );
  }
}

};