// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "QueryObject.h"
#include "DogObject.h"
#include "HumanObject.h"
#include "PetObject.h"
#include "CatOrDogObject.h"
#include "ArgumentsObject.h"
#include "ResourceObject.h"

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

Query::Query(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
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
		{ R"gql(dog)gql"sv, [this](service::ResolverParams&& params) { return resolveDog(std::move(params)); } },
		{ R"gql(pet)gql"sv, [this](service::ResolverParams&& params) { return resolvePet(std::move(params)); } },
		{ R"gql(human)gql"sv, [this](service::ResolverParams&& params) { return resolveHuman(std::move(params)); } },
		{ R"gql(findDog)gql"sv, [this](service::ResolverParams&& params) { return resolveFindDog(std::move(params)); } },
		{ R"gql(catOrDog)gql"sv, [this](service::ResolverParams&& params) { return resolveCatOrDog(std::move(params)); } },
		{ R"gql(resource)gql"sv, [this](service::ResolverParams&& params) { return resolveResource(std::move(params)); } },
		{ R"gql(arguments)gql"sv, [this](service::ResolverParams&& params) { return resolveArguments(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(booleanList)gql"sv, [this](service::ResolverParams&& params) { return resolveBooleanList(std::move(params)); } }
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

service::AwaitableResolver Query::resolveDog(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getDog(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Dog>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveHuman(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getHuman(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Human>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolvePet(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getPet(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Pet>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveCatOrDog(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getCatOrDog(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<CatOrDog>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveArguments(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getArguments(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Arguments>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveResource(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getResource(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Resource>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveFindDog(service::ResolverParams&& params) const
{
	auto argComplex = service::ModifiedArgument<validation::ComplexInput>::require<service::TypeModifier::Nullable>("complex", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getFindDog(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argComplex));
	resolverLock.unlock();

	return service::ModifiedResult<Dog>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveBooleanList(service::ResolverParams&& params) const
{
	auto argBooleanListArg = service::ModifiedArgument<bool>::require<service::TypeModifier::Nullable, service::TypeModifier::List>("booleanListArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getBooleanList(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argBooleanListArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolve_typename(service::ResolverParams&& params) const
{
	return service::Result<std::string>::convert(std::string{ R"gql(Query)gql" }, std::move(params));
}

} // namespace object

void AddQueryDetails(const std::shared_ptr<schema::ObjectType>& typeQuery, const std::shared_ptr<schema::Schema>& schema)
{
	typeQuery->AddFields({
		schema::Field::Make(R"gql(dog)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Dog)gql"sv)),
		schema::Field::Make(R"gql(human)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Human)gql"sv)),
		schema::Field::Make(R"gql(pet)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Pet)gql"sv)),
		schema::Field::Make(R"gql(catOrDog)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(CatOrDog)gql"sv)),
		schema::Field::Make(R"gql(arguments)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Arguments)gql"sv)),
		schema::Field::Make(R"gql(resource)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Resource)gql"sv)),
		schema::Field::Make(R"gql(findDog)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Dog)gql"sv), {
			schema::InputValue::Make(R"gql(complex)gql"sv, R"md()md"sv, schema->LookupType(R"gql(ComplexInput)gql"sv), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(booleanList)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Boolean)gql"sv), {
			schema::InputValue::Make(R"gql(booleanListArg)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv))), R"gql()gql"sv)
		})
	});
}

} // namespace graphql::validation
