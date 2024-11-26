#include "TestServerWrapper.h"

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>

namespace graphql::test_server {

  std::unique_ptr<TestServerWrapper> create_test_server() noexcept
  {
    auto result = std::make_unique<TestServerWrapper>();
    auto query = std::make_shared<Query>();
    auto mutation = std::make_shared<Mutation>();
    auto subscription = std::make_shared<Subscription>();
    result->service = std::make_shared<Operations>(std::move(query),
                                                   std::move(mutation),
                                                   std::move(subscription));
    return result;
  }

  Telemetry::Telemetry(
          response::IdType&& telemetryId, std::string&& when, std::string&& name, int sensorId, double  sensorValue)
          : _telemetryId(std::move(telemetryId))
          , _when(std::make_shared<response::Value>(std::move(when)))
          , _name(std::make_shared<response::Value>(std::move(name)))
          , _sensorId(sensorId)
          , _sensorValue(sensorValue)
  {
  }

  service::AwaitableScalar<response::IdType> Telemetry::getId() const noexcept
  {
    return _telemetryId;
  }

  std::shared_ptr<const response::Value> Telemetry::getWhen() const noexcept
  {
    return _when;
  }

  std::shared_ptr<const response::Value> Telemetry::getName() const noexcept
  {
    return _name;
  }

  int Telemetry::getSensorId() const noexcept
  {
    return _sensorId;
  }
  double Telemetry::getSensorValue() const noexcept
  {
    return _sensorValue;
  }
  std::future<std::shared_ptr<object::Telemetry>> Query::getQueryTelemetry(const service::FieldParams& )
  {
    auto spThis = shared_from_this();
    return std::async(
    std::launch::async,
    [this, spThis]()  {
        auto obj= std::make_shared<test_server::Telemetry>(response::IdType("123"),"123","123",2,2.5);
        return std::make_shared<object::Telemetry>(obj);
    });
  }
  bool Mutation::applyMutationTelemetry(int sensorId, double sensorValue)
  {
    std::cout << "sensorId " << sensorId << " sensorValue " << sensorValue << std::endl;
    return true;
  }
  std::shared_ptr<object::Telemetry> Subscription::getSubscriptionTelemetry()
  {
    auto obj= std::make_shared<test_server::Telemetry>(response::IdType("123"),"123","123",2,2.5);
    return std::make_shared<object::Telemetry>(obj);
  }
}