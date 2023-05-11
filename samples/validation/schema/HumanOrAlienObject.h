// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef HUMANORALIENOBJECT_H
#define HUMANORALIENOBJECT_H

#include "ValidationSchema.h"

namespace graphql::validation::object {

class [[nodiscard]] HumanOrAlien final
	: public service::Object
{
private:
	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		[[nodiscard]] virtual service::TypeNames getTypeNames() const noexcept = 0;
		[[nodiscard]] virtual service::ResolverMap getResolvers() const noexcept = 0;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		explicit Model(std::shared_ptr<T> pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] service::TypeNames getTypeNames() const noexcept final
		{
			return _pimpl->getTypeNames();
		}

		[[nodiscard]] service::ResolverMap getResolvers() const noexcept final
		{
			return _pimpl->getResolvers();
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			_pimpl->beginSelectionSet(params);
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			_pimpl->endSelectionSet(params);
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	explicit HumanOrAlien(std::unique_ptr<const Concept> pimpl) noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	explicit HumanOrAlien(std::shared_ptr<T> pimpl) noexcept
		: HumanOrAlien { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
		static_assert(T::template implements<HumanOrAlien>(), "HumanOrAlien is not implemented");
	}
};

} // namespace graphql::validation::object

#endif // HUMANORALIENOBJECT_H
