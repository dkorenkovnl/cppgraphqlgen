// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "ArgumentsObject.h"

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

Arguments::Arguments(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Arguments::getTypeNames() const noexcept
{
	return {
		R"gql(Arguments)gql"sv
	};
}

service::ResolverMap Arguments::getResolvers() const noexcept
{
	return {
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(intArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveIntArgField(std::move(params)); } },
		{ R"gql(multipleReqs)gql"sv, [this](service::ResolverParams&& params) { return resolveMultipleReqs(std::move(params)); } },
		{ R"gql(floatArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveFloatArgField(std::move(params)); } },
		{ R"gql(booleanArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveBooleanArgField(std::move(params)); } },
		{ R"gql(booleanListArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveBooleanListArgField(std::move(params)); } },
		{ R"gql(nonNullBooleanArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveNonNullBooleanArgField(std::move(params)); } },
		{ R"gql(nonNullBooleanListField)gql"sv, [this](service::ResolverParams&& params) { return resolveNonNullBooleanListField(std::move(params)); } },
		{ R"gql(optionalNonNullBooleanArgField)gql"sv, [this](service::ResolverParams&& params) { return resolveOptionalNonNullBooleanArgField(std::move(params)); } }
	};
}

void Arguments::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Arguments::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Arguments::resolveMultipleReqs(service::ResolverParams&& params) const
{
	auto argX = service::ModifiedArgument<int>::require("x", params.arguments);
	auto argY = service::ModifiedArgument<int>::require("y", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getMultipleReqs(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argX), std::move(argY));
	resolverLock.unlock();

	return service::ModifiedResult<int>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveBooleanArgField(service::ResolverParams&& params) const
{
	auto argBooleanArg = service::ModifiedArgument<bool>::require<service::TypeModifier::Nullable>("booleanArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getBooleanArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argBooleanArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveFloatArgField(service::ResolverParams&& params) const
{
	auto argFloatArg = service::ModifiedArgument<double>::require<service::TypeModifier::Nullable>("floatArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getFloatArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argFloatArg));
	resolverLock.unlock();

	return service::ModifiedResult<double>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveIntArgField(service::ResolverParams&& params) const
{
	auto argIntArg = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>("intArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getIntArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argIntArg));
	resolverLock.unlock();

	return service::ModifiedResult<int>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveNonNullBooleanArgField(service::ResolverParams&& params) const
{
	auto argNonNullBooleanArg = service::ModifiedArgument<bool>::require("nonNullBooleanArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getNonNullBooleanArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argNonNullBooleanArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveNonNullBooleanListField(service::ResolverParams&& params) const
{
	auto argNonNullBooleanListArg = service::ModifiedArgument<bool>::require<service::TypeModifier::Nullable, service::TypeModifier::List>("nonNullBooleanListArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getNonNullBooleanListField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argNonNullBooleanListArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert<service::TypeModifier::Nullable, service::TypeModifier::List>(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveBooleanListArgField(service::ResolverParams&& params) const
{
	auto argBooleanListArg = service::ModifiedArgument<bool>::require<service::TypeModifier::List, service::TypeModifier::Nullable>("booleanListArg", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getBooleanListArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argBooleanListArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert<service::TypeModifier::Nullable, service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolveOptionalNonNullBooleanArgField(service::ResolverParams&& params) const
{
	static const auto defaultArguments = []()
	{
		response::Value values(response::Type::Map);
		response::Value entry;

		entry = response::Value(false);
		values.emplace_back("optionalBooleanArg", std::move(entry));

		return values;
	}();

	auto pairOptionalBooleanArg = service::ModifiedArgument<bool>::find("optionalBooleanArg", params.arguments);
	auto argOptionalBooleanArg = (pairOptionalBooleanArg.second
		? std::move(pairOptionalBooleanArg.first)
		: service::ModifiedArgument<bool>::require("optionalBooleanArg", defaultArguments));
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getOptionalNonNullBooleanArgField(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argOptionalBooleanArg));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Arguments::resolve_typename(service::ResolverParams&& params) const
{
	return service::ModifiedResult<std::string>::convert(std::string{ R"gql(Arguments)gql" }, std::move(params));
}

} // namespace object

void AddArgumentsDetails(const std::shared_ptr<schema::ObjectType>& typeArguments, const std::shared_ptr<schema::Schema>& schema)
{
	typeArguments->AddFields({
		schema::Field::Make(R"gql(multipleReqs)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Int)gql"sv)), {
			schema::InputValue::Make(R"gql(x)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Int)gql"sv)), R"gql()gql"sv),
			schema::InputValue::Make(R"gql(y)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Int)gql"sv)), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(booleanArgField)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Boolean)gql"sv), {
			schema::InputValue::Make(R"gql(booleanArg)gql"sv, R"md()md"sv, schema->LookupType(R"gql(Boolean)gql"sv), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(floatArgField)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Float)gql"sv), {
			schema::InputValue::Make(R"gql(floatArg)gql"sv, R"md()md"sv, schema->LookupType(R"gql(Float)gql"sv), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(intArgField)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(Int)gql"sv), {
			schema::InputValue::Make(R"gql(intArg)gql"sv, R"md()md"sv, schema->LookupType(R"gql(Int)gql"sv), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(nonNullBooleanArgField)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), {
			schema::InputValue::Make(R"gql(nonNullBooleanArg)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(nonNullBooleanListField)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv))), {
			schema::InputValue::Make(R"gql(nonNullBooleanListArg)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv))), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(booleanListArgField)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::LIST, schema->LookupType(R"gql(Boolean)gql"sv)), {
			schema::InputValue::Make(R"gql(booleanListArg)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->LookupType(R"gql(Boolean)gql"sv))), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(optionalNonNullBooleanArgField)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), {
			schema::InputValue::Make(R"gql(optionalBooleanArg)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), R"gql(false)gql"sv)
		})
	});
}

} // namespace graphql::validation
