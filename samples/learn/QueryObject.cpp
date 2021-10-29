// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "StarWarsObjects.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::learn {
namespace object {

Query::Query()
	: service::Object({
		"Query"
	}, {
		{ R"gql(hero)gql"sv, [this](service::ResolverParams&& params) { return resolveHero(std::move(params)); } },
		{ R"gql(droid)gql"sv, [this](service::ResolverParams&& params) { return resolveDroid(std::move(params)); } },
		{ R"gql(human)gql"sv, [this](service::ResolverParams&& params) { return resolveHuman(std::move(params)); } },
		{ R"gql(__type)gql"sv, [this](service::ResolverParams&& params) { return resolve_type(std::move(params)); } },
		{ R"gql(__schema)gql"sv, [this](service::ResolverParams&& params) { return resolve_schema(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	})
	, _schema(GetSchema())
{
}

service::FieldResult<std::shared_ptr<service::Object>> Query::getHero(service::FieldParams&&, std::optional<Episode>&&) const
{
	throw std::runtime_error(R"ex(Query::getHero is not implemented)ex");
}

std::future<service::ResolverResult> Query::resolveHero(service::ResolverParams&& params)
{
	auto argEpisode = service::ModifiedArgument<learn::Episode>::require<service::TypeModifier::Nullable>("episode", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getHero(service::FieldParams(std::move(params), std::move(directives)), std::move(argEpisode));
	resolverLock.unlock();

	return service::ModifiedResult<service::Object>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<std::shared_ptr<Human>> Query::getHuman(service::FieldParams&&, response::StringType&&) const
{
	throw std::runtime_error(R"ex(Query::getHuman is not implemented)ex");
}

std::future<service::ResolverResult> Query::resolveHuman(service::ResolverParams&& params)
{
	auto argId = service::ModifiedArgument<response::StringType>::require("id", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getHuman(service::FieldParams(std::move(params), std::move(directives)), std::move(argId));
	resolverLock.unlock();

	return service::ModifiedResult<Human>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<std::shared_ptr<Droid>> Query::getDroid(service::FieldParams&&, response::StringType&&) const
{
	throw std::runtime_error(R"ex(Query::getDroid is not implemented)ex");
}

std::future<service::ResolverResult> Query::resolveDroid(service::ResolverParams&& params)
{
	auto argId = service::ModifiedArgument<response::StringType>::require("id", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getDroid(service::FieldParams(std::move(params), std::move(directives)), std::move(argId));
	resolverLock.unlock();

	return service::ModifiedResult<Droid>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<service::ResolverResult> Query::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Query)gql" }, std::move(params));
}

std::future<service::ResolverResult> Query::resolve_schema(service::ResolverParams&& params)
{
	return service::ModifiedResult<service::Object>::convert(std::static_pointer_cast<service::Object>(std::make_shared<introspection::Schema>(_schema)), std::move(params));
}

std::future<service::ResolverResult> Query::resolve_type(service::ResolverParams&& params)
{
	auto argName = service::ModifiedArgument<response::StringType>::require("name", params.arguments);
	const auto& baseType = _schema->LookupType(argName);
	std::shared_ptr<introspection::object::Type> result { baseType ? std::make_shared<introspection::Type>(baseType) : nullptr };

	return service::ModifiedResult<introspection::object::Type>::convert<service::TypeModifier::Nullable>(result, std::move(params));
}

} // namespace object

void AddQueryDetails(std::shared_ptr<schema::ObjectType> typeQuery, const std::shared_ptr<schema::Schema>& schema)
{
	typeQuery->AddFields({
		schema::Field::Make(R"gql(hero)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType("Character"), {
			schema::InputValue::Make(R"gql(episode)gql"sv, R"md()md"sv, schema->LookupType("Episode"), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(human)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType("Human"), {
			schema::InputValue::Make(R"gql(id)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(droid)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType("Droid"), {
			schema::InputValue::Make(R"gql(id)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
		})
	});
}

} // namespace graphql::learn
