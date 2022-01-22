// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef STARWARSSCHEMA_H
#define STARWARSSCHEMA_H

#include "graphqlservice/internal/Schema.h"

// Check if the library version is compatible with schemagen 4.1.0
static_assert(graphql::internal::MajorVersion == 4, "regenerate with schemagen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 1, "regenerate with schemagen: minor version mismatch");

#include <memory>
#include <string>
#include <vector>

namespace graphql {
namespace learn {

enum class Episode
{
	NEW_HOPE,
	EMPIRE,
	JEDI
};

struct ReviewInput
{
	int stars {};
	std::optional<std::string> commentary {};
};

namespace object {

class Character;

class Human;
class Droid;
class Query;
class Review;
class Mutation;

} // namespace object

class Operations final
	: public service::Request
{
public:
	explicit Operations(std::shared_ptr<object::Query> query, std::shared_ptr<object::Mutation> mutation);

	template <class TQuery, class TMutation>
	explicit Operations(std::shared_ptr<TQuery> query, std::shared_ptr<TMutation> mutation)
		: Operations { std::make_shared<object::Query>(std::move(query)), std::make_shared<object::Mutation>(std::move(mutation)) }
	{
	}

private:
	std::shared_ptr<object::Query> _query;
	std::shared_ptr<object::Mutation> _mutation;
};

void AddCharacterDetails(const std::shared_ptr<schema::InterfaceType>& typeCharacter, const std::shared_ptr<schema::Schema>& schema);

void AddHumanDetails(const std::shared_ptr<schema::ObjectType>& typeHuman, const std::shared_ptr<schema::Schema>& schema);
void AddDroidDetails(const std::shared_ptr<schema::ObjectType>& typeDroid, const std::shared_ptr<schema::Schema>& schema);
void AddQueryDetails(const std::shared_ptr<schema::ObjectType>& typeQuery, const std::shared_ptr<schema::Schema>& schema);
void AddReviewDetails(const std::shared_ptr<schema::ObjectType>& typeReview, const std::shared_ptr<schema::Schema>& schema);
void AddMutationDetails(const std::shared_ptr<schema::ObjectType>& typeMutation, const std::shared_ptr<schema::Schema>& schema);

std::shared_ptr<schema::Schema> GetSchema();

} // namespace learn
} // namespace graphql

#endif // STARWARSSCHEMA_H
