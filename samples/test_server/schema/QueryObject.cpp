// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "QueryObject.h"
#include "TelemetryObject.h"

#include "graphqlservice/internal/Introspection.h"

#include "graphqlservice/introspection/SchemaObject.h"
#include "graphqlservice/introspection/TypeObject.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::test_server {
namespace object {

Query::Query(std::unique_ptr<const Concept> pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _schema { GetSchema() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Query::getTypeNames() const noexcept
{
	return {
		R"gql(Query)gql"sv
	};
}

service::ResolverMap Query::getResolvers() const noexcept
{
	return {
		{ R"gql(__type)gql"sv, [this](service::ResolverParams&& params) { return resolve_type(std::move(params)); } },
		{ R"gql(__schema)gql"sv, [this](service::ResolverParams&& params) { return resolve_schema(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(queryTelemetry)gql"sv, [this](service::ResolverParams&& params) { return resolveQueryTelemetry(std::move(params)); } }
	};
}

void Query::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Query::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Query::resolveQueryTelemetry(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	service::SelectionSetParams selectionSetParams { static_cast<const service::SelectionSetParams&>(params) };
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getQueryTelemetry(service::FieldParams { std::move(selectionSetParams), std::move(directives) });
	resolverLock.unlock();

	return service::ModifiedResult<Telemetry>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolve_typename(service::ResolverParams&& params) const
{
	return service::Result<std::string>::convert(std::string{ R"gql(Query)gql" }, std::move(params));
}

service::AwaitableResolver Query::resolve_schema(service::ResolverParams&& params) const
{
	return service::Result<service::Object>::convert(std::static_pointer_cast<service::Object>(std::make_shared<introspection::object::Schema>(std::make_shared<introspection::Schema>(_schema))), std::move(params));
}

service::AwaitableResolver Query::resolve_type(service::ResolverParams&& params) const
{
	auto argName = service::ModifiedArgument<std::string>::require("name", params.arguments);
	const auto& baseType = _schema->LookupType(argName);
	std::shared_ptr<introspection::object::Type> result { baseType ? std::make_shared<introspection::object::Type>(std::make_shared<introspection::Type>(baseType)) : nullptr };

	return service::ModifiedResult<introspection::object::Type>::convert<service::TypeModifier::Nullable>(result, std::move(params));
}

} // namespace object

void AddQueryDetails(const std::shared_ptr<schema::ObjectType>& typeQuery, const std::shared_ptr<schema::Schema>& schema)
{
	typeQuery->AddFields({
		schema::Field::Make(R"gql(queryTelemetry)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Telemetry)gql"sv)))
	});
}

} // namespace graphql::test_server
