// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "TodayObjects.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <array>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <vector>

using namespace std::literals;

namespace graphql {
namespace service {

static const std::array<std::string_view, 4> s_namesTaskState = {
	"New",
	"Started",
	"Complete",
	"Unassigned"
};

template <>
today::TaskState ModifiedArgument<today::TaskState>::convert(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { "not a valid TaskState value" } };
	}

	auto itr = std::find(s_namesTaskState.cbegin(), s_namesTaskState.cend(), value.get<response::StringType>());

	if (itr == s_namesTaskState.cend())
	{
		throw service::schema_exception { { "not a valid TaskState value" } };
	}

	return static_cast<today::TaskState>(itr - s_namesTaskState.cbegin());
}

template <>
std::future<service::ResolverResult> ModifiedResult<today::TaskState>::convert(service::FieldResult<today::TaskState>&& result, ResolverParams&& params)
{
	return resolve(std::move(result), std::move(params),
		[](today::TaskState&& value, const ResolverParams&)
		{
			response::Value result(response::Type::EnumValue);

			result.set<response::StringType>(std::string(s_namesTaskState[static_cast<size_t>(value)]));

			return result;
		});
}

template <>
today::CompleteTaskInput ModifiedArgument<today::CompleteTaskInput>::convert(const response::Value& value)
{
	const auto defaultValue = []()
	{
		response::Value values(response::Type::Map);
		response::Value entry;

		entry = response::Value(true);
		values.emplace_back("isComplete", std::move(entry));

		return values;
	}();

	auto valueId = service::ModifiedArgument<response::IdType>::require("id", value);
	auto pairIsComplete = service::ModifiedArgument<response::BooleanType>::find<service::TypeModifier::Nullable>("isComplete", value);
	auto valueIsComplete = (pairIsComplete.second
		? std::move(pairIsComplete.first)
		: service::ModifiedArgument<response::BooleanType>::require<service::TypeModifier::Nullable>("isComplete", defaultValue));
	auto valueClientMutationId = service::ModifiedArgument<response::StringType>::require<service::TypeModifier::Nullable>("clientMutationId", value);

	return {
		std::move(valueId),
		std::move(valueIsComplete),
		std::move(valueClientMutationId)
	};
}

} /* namespace service */

namespace today {

Operations::Operations(std::shared_ptr<object::Query> query, std::shared_ptr<object::Mutation> mutation, std::shared_ptr<object::Subscription> subscription)
	: service::Request({
		{ "query", query },
		{ "mutation", mutation },
		{ "subscription", subscription }
	}, GetSchema())
	, _query(std::move(query))
	, _mutation(std::move(mutation))
	, _subscription(std::move(subscription))
{
}

void AddTypesToSchema(const std::shared_ptr<schema::Schema>& schema)
{
	schema->AddType(R"gql(ItemCursor)gql"sv, schema::ScalarType::Make(R"gql(ItemCursor)gql"sv, R"md()md"));
	schema->AddType(R"gql(DateTime)gql"sv, schema::ScalarType::Make(R"gql(DateTime)gql"sv, R"md()md"));
	auto typeTaskState = schema::EnumType::Make(R"gql(TaskState)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(TaskState)gql"sv, typeTaskState);
	auto typeCompleteTaskInput = schema::InputObjectType::Make(R"gql(CompleteTaskInput)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(CompleteTaskInput)gql"sv, typeCompleteTaskInput);
	auto typeUnionType = schema::UnionType::Make(R"gql(UnionType)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(UnionType)gql"sv, typeUnionType);
	auto typeNode = schema::InterfaceType::Make(R"gql(Node)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(Node)gql"sv, typeNode);
	auto typeQuery = schema::ObjectType::Make(R"gql(Query)gql"sv, R"md()md");
	schema->AddType(R"gql(Query)gql"sv, typeQuery);
	auto typePageInfo = schema::ObjectType::Make(R"gql(PageInfo)gql"sv, R"md()md");
	schema->AddType(R"gql(PageInfo)gql"sv, typePageInfo);
	auto typeAppointmentEdge = schema::ObjectType::Make(R"gql(AppointmentEdge)gql"sv, R"md()md");
	schema->AddType(R"gql(AppointmentEdge)gql"sv, typeAppointmentEdge);
	auto typeAppointmentConnection = schema::ObjectType::Make(R"gql(AppointmentConnection)gql"sv, R"md()md");
	schema->AddType(R"gql(AppointmentConnection)gql"sv, typeAppointmentConnection);
	auto typeTaskEdge = schema::ObjectType::Make(R"gql(TaskEdge)gql"sv, R"md()md");
	schema->AddType(R"gql(TaskEdge)gql"sv, typeTaskEdge);
	auto typeTaskConnection = schema::ObjectType::Make(R"gql(TaskConnection)gql"sv, R"md()md");
	schema->AddType(R"gql(TaskConnection)gql"sv, typeTaskConnection);
	auto typeFolderEdge = schema::ObjectType::Make(R"gql(FolderEdge)gql"sv, R"md()md");
	schema->AddType(R"gql(FolderEdge)gql"sv, typeFolderEdge);
	auto typeFolderConnection = schema::ObjectType::Make(R"gql(FolderConnection)gql"sv, R"md()md");
	schema->AddType(R"gql(FolderConnection)gql"sv, typeFolderConnection);
	auto typeCompleteTaskPayload = schema::ObjectType::Make(R"gql(CompleteTaskPayload)gql"sv, R"md()md");
	schema->AddType(R"gql(CompleteTaskPayload)gql"sv, typeCompleteTaskPayload);
	auto typeMutation = schema::ObjectType::Make(R"gql(Mutation)gql"sv, R"md()md");
	schema->AddType(R"gql(Mutation)gql"sv, typeMutation);
	auto typeSubscription = schema::ObjectType::Make(R"gql(Subscription)gql"sv, R"md()md");
	schema->AddType(R"gql(Subscription)gql"sv, typeSubscription);
	auto typeAppointment = schema::ObjectType::Make(R"gql(Appointment)gql"sv, R"md()md");
	schema->AddType(R"gql(Appointment)gql"sv, typeAppointment);
	auto typeTask = schema::ObjectType::Make(R"gql(Task)gql"sv, R"md()md");
	schema->AddType(R"gql(Task)gql"sv, typeTask);
	auto typeFolder = schema::ObjectType::Make(R"gql(Folder)gql"sv, R"md()md");
	schema->AddType(R"gql(Folder)gql"sv, typeFolder);
	auto typeNestedType = schema::ObjectType::Make(R"gql(NestedType)gql"sv, R"md()md");
	schema->AddType(R"gql(NestedType)gql"sv, typeNestedType);
	auto typeExpensive = schema::ObjectType::Make(R"gql(Expensive)gql"sv, R"md()md");
	schema->AddType(R"gql(Expensive)gql"sv, typeExpensive);

	typeTaskState->AddEnumValues({
		{ service::s_namesTaskState[static_cast<size_t>(today::TaskState::New)], R"md()md"sv, std::nullopt },
		{ service::s_namesTaskState[static_cast<size_t>(today::TaskState::Started)], R"md()md"sv, std::nullopt },
		{ service::s_namesTaskState[static_cast<size_t>(today::TaskState::Complete)], R"md()md"sv, std::nullopt },
		{ service::s_namesTaskState[static_cast<size_t>(today::TaskState::Unassigned)], R"md()md"sv, std::make_optional(R"md(Need to deprecate an [enum value](http://spec.graphql.org/June2018/#sec-Deprecation))md"sv) }
	});

	typeCompleteTaskInput->AddInputValues({
		schema::InputValue::Make(R"gql(id)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("ID")), R"gql()gql"sv),
		schema::InputValue::Make(R"gql(isComplete)gql"sv, R"md()md"sv, schema->LookupType("Boolean"), R"gql(true)gql"sv),
		schema::InputValue::Make(R"gql(clientMutationId)gql"sv, R"md()md"sv, schema->LookupType("String"), R"gql()gql"sv)
	});

	typeUnionType->AddPossibleTypes({
		schema->LookupType(R"gql(Appointment)gql"sv),
		schema->LookupType(R"gql(Task)gql"sv),
		schema->LookupType(R"gql(Folder)gql"sv)
	});

	typeNode->AddFields({
		schema::Field::Make(R"gql(id)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("ID")))
	});

	AddQueryDetails(typeQuery, schema);
	AddPageInfoDetails(typePageInfo, schema);
	AddAppointmentEdgeDetails(typeAppointmentEdge, schema);
	AddAppointmentConnectionDetails(typeAppointmentConnection, schema);
	AddTaskEdgeDetails(typeTaskEdge, schema);
	AddTaskConnectionDetails(typeTaskConnection, schema);
	AddFolderEdgeDetails(typeFolderEdge, schema);
	AddFolderConnectionDetails(typeFolderConnection, schema);
	AddCompleteTaskPayloadDetails(typeCompleteTaskPayload, schema);
	AddMutationDetails(typeMutation, schema);
	AddSubscriptionDetails(typeSubscription, schema);
	AddAppointmentDetails(typeAppointment, schema);
	AddTaskDetails(typeTask, schema);
	AddFolderDetails(typeFolder, schema);
	AddNestedTypeDetails(typeNestedType, schema);
	AddExpensiveDetails(typeExpensive, schema);

	schema->AddDirective(schema::Directive::Make(R"gql(id)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FIELD_DEFINITION
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(subscriptionTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::SUBSCRIPTION
	}, {
		schema::InputValue::Make(R"gql(field)gql"sv, R"md()md"sv, schema->LookupType("String"), R"gql()gql"sv)
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(queryTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::QUERY
	}, {
		schema::InputValue::Make(R"gql(query)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(fieldTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FIELD
	}, {
		schema::InputValue::Make(R"gql(field)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(fragmentDefinitionTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FRAGMENT_DEFINITION
	}, {
		schema::InputValue::Make(R"gql(fragmentDefinition)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(fragmentSpreadTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FRAGMENT_SPREAD
	}, {
		schema::InputValue::Make(R"gql(fragmentSpread)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
	}));
	schema->AddDirective(schema::Directive::Make(R"gql(inlineFragmentTag)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::INLINE_FRAGMENT
	}, {
		schema::InputValue::Make(R"gql(inlineFragment)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")), R"gql()gql"sv)
	}));

	schema->AddQueryType(typeQuery);
	schema->AddMutationType(typeMutation);
	schema->AddSubscriptionType(typeSubscription);
}

std::shared_ptr<schema::Schema> GetSchema()
{
	static std::weak_ptr<schema::Schema> s_wpSchema;
	auto schema = s_wpSchema.lock();

	if (!schema)
	{
		schema = std::make_shared<schema::Schema>(true);
		introspection::AddTypesToSchema(schema);
		AddTypesToSchema(schema);
		s_wpSchema = schema;
	}

	return schema;
}

} /* namespace today */
} /* namespace graphql */
