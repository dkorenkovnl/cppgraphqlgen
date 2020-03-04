// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef VALIDATION_H
#define VALIDATION_H

#include "graphqlservice/GraphQLService.h"
#include "graphqlservice/IntrospectionSchema.h"

namespace graphql::service {

using ValidateType = response::Value;

struct ValidateArgument
{
	bool defaultValue = false;
	ValidateType type;
};

using ValidateTypeFieldArguments = std::map<std::string, ValidateArgument>;

struct ValidateTypeField
{
	ValidateType returnType;
	ValidateTypeFieldArguments arguments;
};

using ValidateDirectiveArguments = std::map<std::string, ValidateArgument>;

struct ValidateDirective
{
	std::set<introspection::DirectiveLocation> locations;
	ValidateDirectiveArguments arguments;
};

struct ValidateArgumentVariable
{
	bool operator==(const ValidateArgumentVariable& other) const;

	std::string name;
};

struct ValidateArgumentEnumValue
{
	bool operator==(const ValidateArgumentEnumValue& other) const;

	std::string value;
};

struct ValidateArgumentValue;

struct ValidateArgumentValuePtr
{
	bool operator==(const ValidateArgumentValuePtr& other) const;

	std::unique_ptr<ValidateArgumentValue> value;
	schema_location position;
};

struct ValidateArgumentList
{
	bool operator==(const ValidateArgumentList& other) const;

	std::vector<ValidateArgumentValuePtr> values;
};

struct ValidateArgumentMap
{
	bool operator==(const ValidateArgumentMap& other) const;

	std::map<std::string, ValidateArgumentValuePtr> values;
};

using ValidateArgumentVariant = std::variant<
	ValidateArgumentVariable,
	response::IntType,
	response::FloatType,
	response::StringType,
	response::BooleanType,
	ValidateArgumentEnumValue,
	ValidateArgumentList,
	ValidateArgumentMap>;

struct ValidateArgumentValue
{
	ValidateArgumentValue(ValidateArgumentVariable&& value);
	ValidateArgumentValue(response::IntType value);
	ValidateArgumentValue(response::FloatType value);
	ValidateArgumentValue(response::StringType&& value);
	ValidateArgumentValue(response::BooleanType value);
	ValidateArgumentValue(ValidateArgumentEnumValue&& value);
	ValidateArgumentValue(ValidateArgumentList&& value);
	ValidateArgumentValue(ValidateArgumentMap&& value);

	ValidateArgumentVariant data;
};

// ValidateArgumentValueVisitor visits the AST and builds a record of a field return type and map
// of the arguments for comparison to see if 2 fields with the same result name can be merged.
class ValidateArgumentValueVisitor
{
public:
	ValidateArgumentValueVisitor();

	void visit(const peg::ast_node& value);

	ValidateArgumentValuePtr getArgumentValue();

private:
	void visitVariable(const peg::ast_node& variable);
	void visitIntValue(const peg::ast_node& intValue);
	void visitFloatValue(const peg::ast_node& floatValue);
	void visitStringValue(const peg::ast_node& stringValue);
	void visitBooleanValue(const peg::ast_node& booleanValue);
	void visitNullValue(const peg::ast_node& nullValue);
	void visitEnumValue(const peg::ast_node& enumValue);
	void visitListValue(const peg::ast_node& listValue);
	void visitObjectValue(const peg::ast_node& objectValue);

	ValidateArgumentValuePtr _argumentValue;
};

using ValidateFieldArguments = std::map<std::string, ValidateArgumentValuePtr>;

struct ValidateField
{
	ValidateField(std::string&& returnType, std::optional<std::string>&& objectType, const std::string& fieldName, ValidateFieldArguments&& arguments);

	bool operator==(const ValidateField& other) const;

	std::string returnType;
	std::optional<std::string> objectType;
	std::string fieldName;
	ValidateFieldArguments arguments;
};

// ValidateExecutableVisitor visits the AST and validates that it is executable against the service schema.
class ValidateExecutableVisitor
{
public:
	ValidateExecutableVisitor(const Request& service);

	void visit(const peg::ast_node& root);

	std::vector<schema_error> getStructuredErrors();

private:
	response::Value executeQuery(std::string_view query) const;

	static ValidateTypeFieldArguments getArguments(response::ListType&& argumentsMember);

	using FieldTypes = std::map<std::string, ValidateTypeField>;
	using TypeFields = std::map<std::string, FieldTypes>;

	std::optional<introspection::TypeKind> getScopedTypeKind() const;
	TypeFields::const_iterator getScopedTypeFields();
	static std::string getFieldType(const FieldTypes& fields, const std::string& name);
	static std::string getWrappedFieldType(const FieldTypes& fields, const std::string& name);
	static std::string getWrappedFieldType(const ValidateType& returnType);

	void visitFragmentDefinition(const peg::ast_node& fragmentDefinition);
	void visitOperationDefinition(const peg::ast_node& operationDefinition);

	void visitSelection(const peg::ast_node& selection);

	void visitField(const peg::ast_node& field);
	void visitFragmentSpread(const peg::ast_node& fragmentSpread);
	void visitInlineFragment(const peg::ast_node& inlineFragment);

	void visitDirectives(introspection::DirectiveLocation location, const peg::ast_node& directives);

	const Request& _service;
	std::vector<schema_error> _errors;

	using OperationTypes = std::map<std::string_view, std::string>;
	using TypeKinds = std::map<std::string, introspection::TypeKind>;
	using Directives = std::map<std::string, ValidateDirective>;
	using ExecutableNodes = std::map<std::string, const peg::ast_node&>;
	using FragmentSet = std::unordered_set<std::string>;

	OperationTypes _operationTypes;
	TypeKinds _typeKinds;
	Directives _directives;

	ExecutableNodes _fragmentDefinitions;
	ExecutableNodes _operationDefinitions;

	FragmentSet _referencedFragments;
	FragmentSet _fragmentStack;
	FragmentSet _fragmentCycles;
	size_t _fieldCount = 0;
	TypeFields _typeFields;
	std::string _scopedType;
	std::map<std::string, ValidateField> _selectionFields;
};

} /* namespace graphql::service */

#endif // VALIDATION_H
