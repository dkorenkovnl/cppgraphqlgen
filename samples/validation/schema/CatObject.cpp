// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "CatObject.h"

#include "graphqlservice/internal/Schema.h"

#include "graphqlservice/introspection/IntrospectionSchema.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::validation {
namespace object {

Cat::Cat(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Cat::getTypeNames() const noexcept
{
	return {
		R"gql(Pet)gql"sv,
		R"gql(CatOrDog)gql"sv,
		R"gql(Cat)gql"sv
	};
}

service::ResolverMap Cat::getResolvers() const noexcept
{
	return {
		{ R"gql(name)gql"sv, [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ R"gql(nickname)gql"sv, [this](service::ResolverParams&& params) { return resolveNickname(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(meowVolume)gql"sv, [this](service::ResolverParams&& params) { return resolveMeowVolume(std::move(params)); } },
		{ R"gql(doesKnowCommand)gql"sv, [this](service::ResolverParams&& params) { return resolveDoesKnowCommand(std::move(params)); } }
	};
}

void Cat::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Cat::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Cat::resolveName(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getName(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<std::string>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Cat::resolveNickname(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getNickname(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<std::string>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Cat::resolveDoesKnowCommand(service::ResolverParams&& params) const
{
	auto argCatCommand = service::ModifiedArgument<CatCommand>::require("catCommand", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getDoesKnowCommand(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argCatCommand));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Cat::resolveMeowVolume(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getMeowVolume(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<int>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Cat::resolve_typename(service::ResolverParams&& params) const
{
	return service::Result<std::string>::convert(std::string{ R"gql(Cat)gql" }, std::move(params));
}

} // namespace object

void AddCatDetails(const std::shared_ptr<schema::ObjectType>& typeCat, const std::shared_ptr<schema::Schema>& schema)
{
	typeCat->AddInterfaces({
		std::static_pointer_cast<const schema::InterfaceType>(schema->LookupType(R"gql(Pet)gql"sv))
	});
	typeCat->AddFields({
		schema::Field::Make(R"gql(name)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(String)gql"sv))),
		schema::Field::Make(R"gql(nickname)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(String)gql"sv)),
		schema::Field::Make(R"gql(doesKnowCommand)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), {
			schema::InputValue::Make(R"gql(catCommand)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(CatCommand)gql"sv)), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(meowVolume)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Int)gql"sv))
	});
}

} // namespace graphql::validation
