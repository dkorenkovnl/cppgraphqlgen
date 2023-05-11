// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef MUTATIONOBJECT_H
#define MUTATIONOBJECT_H

#include "StarWarsSchema.h"

namespace graphql::learn::object {
namespace methods::MutationHas {

template <class TImpl>
concept applyCreateReviewWithParams = requires (TImpl impl, service::FieldParams params, Episode epArg, ReviewInput reviewArg)
{
	{ service::AwaitableObject<std::shared_ptr<Review>> { impl.applyCreateReview(std::move(params), std::move(epArg), std::move(reviewArg)) } };
};

template <class TImpl>
concept applyCreateReview = requires (TImpl impl, Episode epArg, ReviewInput reviewArg)
{
	{ service::AwaitableObject<std::shared_ptr<Review>> { impl.applyCreateReview(std::move(epArg), std::move(reviewArg)) } };
};

template <class TImpl>
concept beginSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.beginSelectionSet(params) };
};

template <class TImpl>
concept endSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.endSelectionSet(params) };
};

} // namespace methods::MutationHas

class [[nodiscard]] Mutation final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveCreateReview(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		[[nodiscard]] virtual service::AwaitableObject<std::shared_ptr<Review>> applyCreateReview(service::FieldParams&& params, Episode&& epArg, ReviewInput&& reviewArg) const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		explicit Model(std::shared_ptr<T> pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] service::AwaitableObject<std::shared_ptr<Review>> applyCreateReview(service::FieldParams&& params, Episode&& epArg, ReviewInput&& reviewArg) const final
		{
			if constexpr (methods::MutationHas::applyCreateReviewWithParams<T>)
			{
				return { _pimpl->applyCreateReview(std::move(params), std::move(epArg), std::move(reviewArg)) };
			}
			else
			{
				static_assert(methods::MutationHas::applyCreateReview<T>, R"msg(Mutation::applyCreateReview is not implemented)msg");
				return { _pimpl->applyCreateReview(std::move(epArg), std::move(reviewArg)) };
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::MutationHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::MutationHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	explicit Mutation(std::unique_ptr<const Concept> pimpl) noexcept;

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	explicit Mutation(std::shared_ptr<T> pimpl) noexcept
		: Mutation { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	[[nodiscard]] static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(Mutation)gql" };
	}
};

} // namespace graphql::learn::object

#endif // MUTATIONOBJECT_H
