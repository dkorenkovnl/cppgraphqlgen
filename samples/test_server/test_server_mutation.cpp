#include "TestServerWrapper.h"

#include "graphqlservice/JSONResponse.h"

#include <cstdio>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace graphql;

int main(int , char** )
{
  const auto server = test_server::create_test_server();
  const auto& service = server->service;

  std::cout << "Created the test_server_mutation..." << std::endl;
  try
  {
    peg::ast ast = R"(mutation {
      mutationTelemetry(sensorId: 1000,sensorValue:0.1)
    })"_graphql;

    std::cout << "Executing subscription..." << std::endl;

    response::Value variables(response::Type::Map);
    auto state = std::make_shared<test_server::RequestState>(1);
    std::cout << response::toJSON(service->resolve({ ast, {}, std::move(variables), {}, state }).get())<< std::endl;

  }
  catch (const std::runtime_error& ex)
  {
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  return 0;
}