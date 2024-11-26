#include "TestServerWrapper.h"

#include "graphqlservice/JSONResponse.h"

#include <cstdio>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace graphql;
using namespace std::literals;

int main(int , char** )
{
  const auto server = test_server::create_test_server();
  const auto& service = server->service;

  std::cout << "Created the test_server_subscription..." << std::endl;
  try
  {
    auto query = peg::parseString(R"(subscription {
		subscriptionTelemetry {
			id: id
			when
			name
			sensorId
			sensorValue
		}
	})");

    response::Value variables(response::Type::Map);
    auto state = std::make_shared<test_server::RequestState>(1);

    response::Value result;
    auto key = service->subscribe({ [&result](response::Value&& response) {
        result = std::move(response);
    },
                                    std::move(query),
                                    ""s,
                                    std::move(variables),
                                    {},
                                    state }).get();
    service->deliver({ "subscriptionTelemetry"sv }).get();
    service->unsubscribe({ key }).get();

    std::cout << response::toJSON(response::Value(result)) << std::endl;

  }
  catch (const std::runtime_error& ex)
  {
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  return 0;
}