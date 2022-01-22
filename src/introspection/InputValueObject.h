// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef INPUTVALUEOBJECT_H
#define INPUTVALUEOBJECT_H

#include "IntrospectionSchema.h"

namespace graphql::introspection::object {

class InputValue final
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveType(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveDefaultValue(service::ResolverParams&& params) const;

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::AwaitableScalar<std::string> getName() const = 0;
		virtual service::AwaitableScalar<std::optional<std::string>> getDescription() const = 0;
		virtual service::AwaitableObject<std::shared_ptr<Type>> getType() const = 0;
		virtual service::AwaitableScalar<std::optional<std::string>> getDefaultValue() const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::AwaitableScalar<std::string> getName() const final
		{
			return { _pimpl->getName() };
		}

		service::AwaitableScalar<std::optional<std::string>> getDescription() const final
		{
			return { _pimpl->getDescription() };
		}

		service::AwaitableObject<std::shared_ptr<Type>> getType() const final
		{
			return { _pimpl->getType() };
		}

		service::AwaitableScalar<std::optional<std::string>> getDefaultValue() const final
		{
			return { _pimpl->getDefaultValue() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<const Concept> _pimpl;

	service::TypeNames getTypeNames() const noexcept;
	service::ResolverMap getResolvers() const noexcept;

public:
	GRAPHQLSERVICE_EXPORT InputValue(std::shared_ptr<introspection::InputValue> pimpl) noexcept;
	GRAPHQLSERVICE_EXPORT ~InputValue();
};

} // namespace graphql::introspection::object

#endif // INPUTVALUEOBJECT_H
