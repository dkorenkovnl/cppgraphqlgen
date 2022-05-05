// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef NESTEDINPUTCLIENT_H
#define NESTEDINPUTCLIENT_H

#include "graphqlservice/GraphQLClient.h"
#include "graphqlservice/GraphQLParse.h"
#include "graphqlservice/GraphQLResponse.h"

#include "graphqlservice/internal/Version.h"

// Check if the library version is compatible with clientgen 4.3.0
static_assert(graphql::internal::MajorVersion == 4, "regenerate with clientgen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 3, "regenerate with clientgen: minor version mismatch");

#include <optional>
#include <string>
#include <vector>

namespace graphql::client {

/// <summary>
/// Operation: query testQuery
/// </summary>
/// <code class="language-graphql">
/// query testQuery($stream: InputABCD!) {
///   control {
///     test(new: $stream) {
///       id
///     }
///   }
/// }
/// </code>
namespace nestedinput {

// Return the original text of the request document.
[[nodiscard]] const std::string& GetRequestText() noexcept;

// Return a pre-parsed, pre-validated request object.
[[nodiscard]] const peg::ast& GetRequestObject() noexcept;

struct [[nodiscard]] InputA
{
	bool a {};
};

struct [[nodiscard]] InputB
{
	double b {};
};

struct InputBC;

struct [[nodiscard]] InputABCD
{
	std::string d {};
	InputA a {};
	InputB b {};
	std::vector<InputBC> bc {};
};

struct [[nodiscard]] InputBC
{
	response::IdType c {};
	InputB b {};
};

} // namespace nestedinput

namespace query::testQuery {

using nestedinput::GetRequestText;
using nestedinput::GetRequestObject;

// Return the name of this operation in the shared request document.
[[nodiscard]] const std::string& GetOperationName() noexcept;

using nestedinput::InputA;
using nestedinput::InputB;
using nestedinput::InputABCD;
using nestedinput::InputBC;

struct [[nodiscard]] Variables
{
	InputABCD stream {};
};

[[nodiscard]] response::Value serializeVariables(Variables&& variables);

struct [[nodiscard]] Response
{
	struct [[nodiscard]] control_Control
	{
		struct [[nodiscard]] test_Output
		{
			std::optional<bool> id {};
		};

		std::optional<test_Output> test {};
	};

	control_Control control {};
};

[[nodiscard]] Response parseResponse(response::Value&& response);

} // namespace query::testQuery
} // namespace graphql::client

#endif // NESTEDINPUTCLIENT_H
