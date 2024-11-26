#pragma once

#ifndef CPPGRAPHQLGEN_TESTSERVERWRAPPER_H
#define CPPGRAPHQLGEN_TESTSERVERWRAPPER_H

#include <atomic>
#include <memory>
#include <stack>
#include <iostream>

#include "TestServerSchema.h"

#include "TestServerSchema.h"
#include "TelemetryObject.h"
#include "QueryObject.h"
#include "SubscriptionObject.h"
#include "MutationObject.h"

namespace graphql::test_server {

  struct TestServerWrapper
  {
      std::shared_ptr<Operations> service {};
  };

  std::unique_ptr<TestServerWrapper> create_test_server() noexcept;

  struct RequestState : service::RequestState
  {
      RequestState(size_t id):requestId(id){}
      const size_t requestId;
  };

  class Telemetry
  {
  public:
      explicit Telemetry(response::IdType&& telemetryId, std::string&& when, std::string&& name, int sensorId, double  sensorValue);
      service::AwaitableScalar<response::IdType> getId() const noexcept;
      std::shared_ptr<const response::Value> getWhen() const noexcept;
      std::shared_ptr<const response::Value> getName() const noexcept;
      int getSensorId() const noexcept;
      double getSensorValue() const noexcept;

  private:
      response::IdType _telemetryId;
      std::shared_ptr<const response::Value> _when;
      std::shared_ptr<const response::Value> _name;
      int _sensorId;
      double _sensorValue;
  };

  class Query : public std::enable_shared_from_this<Query>
  {
  public:
      explicit Query() = default;
      std::future<std::shared_ptr<object::Telemetry>> getQueryTelemetry(const service::FieldParams& params);
  };

  class Mutation
  {
  public:
      explicit Mutation() = default;
      bool applyMutationTelemetry(int sensorId, double sensorValue);
  };

  class Subscription
  {
  public:
      explicit Subscription() = default;
      std::shared_ptr<object::Telemetry> getSubscriptionTelemetry();
  };
}

#endif //CPPGRAPHQLGEN_TESTSERVERWRAPPER_H
