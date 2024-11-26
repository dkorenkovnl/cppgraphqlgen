// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef TESTSERVERSCHEMA_H
#define TESTSERVERSCHEMA_H

#include "graphqlservice/internal/Schema.h"

// Check if the library version is compatible with schemagen 4.5.0
static_assert(graphql::internal::MajorVersion == 4, "regenerate with schemagen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 5, "regenerate with schemagen: minor version mismatch");

#include <array>
#include <memory>
#include <string>
#include <string_view>

namespace graphql {
namespace test_server {
namespace object {

class Telemetry;
class Query;
class Subscription;
class Mutation;

} // namespace object

class [[nodiscard("unnecessary construction")]] Operations final
	: public service::Request
{
public:
	explicit Operations(std::shared_ptr<object::Query> query, std::shared_ptr<object::Mutation> mutation, std::shared_ptr<object::Subscription> subscription);

	template <class TQuery, class TMutation, class TSubscription = service::SubscriptionPlaceholder>
	explicit Operations(std::shared_ptr<TQuery> query, std::shared_ptr<TMutation> mutation, std::shared_ptr<TSubscription> subscription = {})
		: Operations {
			std::make_shared<object::Query>(std::move(query)),
			std::make_shared<object::Mutation>(std::move(mutation)),
			subscription ? std::make_shared<object::Subscription>(std::move(subscription)) : std::shared_ptr<object::Subscription> {}
		}
	{
	}

private:
	std::shared_ptr<object::Query> _query;
	std::shared_ptr<object::Mutation> _mutation;
	std::shared_ptr<object::Subscription> _subscription;
};

void AddTelemetryDetails(const std::shared_ptr<schema::ObjectType>& typeTelemetry, const std::shared_ptr<schema::Schema>& schema);
void AddQueryDetails(const std::shared_ptr<schema::ObjectType>& typeQuery, const std::shared_ptr<schema::Schema>& schema);
void AddSubscriptionDetails(const std::shared_ptr<schema::ObjectType>& typeSubscription, const std::shared_ptr<schema::Schema>& schema);
void AddMutationDetails(const std::shared_ptr<schema::ObjectType>& typeMutation, const std::shared_ptr<schema::Schema>& schema);

std::shared_ptr<schema::Schema> GetSchema();

} // namespace test_server
} // namespace graphql

#endif // TESTSERVERSCHEMA_H
