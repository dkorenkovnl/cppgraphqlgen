// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef TASKEDGEOBJECT_H
#define TASKEDGEOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {
namespace methods::TaskEdgeHas {

template <class TImpl>
concept getNodeWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableObject<std::shared_ptr<Task>> { impl.getNode(std::move(params)) } };
};

template <class TImpl>
concept getNode = requires (TImpl impl)
{
	{ service::AwaitableObject<std::shared_ptr<Task>> { impl.getNode() } };
};

template <class TImpl>
concept getCursorWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<response::Value> { impl.getCursor(std::move(params)) } };
};

template <class TImpl>
concept getCursor = requires (TImpl impl)
{
	{ service::AwaitableScalar<response::Value> { impl.getCursor() } };
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

} // namespace methods::TaskEdgeHas

class [[nodiscard]] TaskEdge final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveNode(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveCursor(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		[[nodiscard]] virtual service::AwaitableObject<std::shared_ptr<Task>> getNode(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<response::Value> getCursor(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		inline Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] inline service::AwaitableObject<std::shared_ptr<Task>> getNode(service::FieldParams&& params) const final
		{
			if constexpr (methods::TaskEdgeHas::getNodeWithParams<T>)
			{
				return { _pimpl->getNode(std::move(params)) };
			}
			else if constexpr (methods::TaskEdgeHas::getNode<T>)
			{
				return { _pimpl->getNode() };
			}
			else
			{
				throw std::runtime_error(R"ex(TaskEdge::getNode is not implemented)ex");
			}
		}

		[[nodiscard]] inline service::AwaitableScalar<response::Value> getCursor(service::FieldParams&& params) const final
		{
			if constexpr (methods::TaskEdgeHas::getCursorWithParams<T>)
			{
				return { _pimpl->getCursor(std::move(params)) };
			}
			else if constexpr (methods::TaskEdgeHas::getCursor<T>)
			{
				return { _pimpl->getCursor() };
			}
			else
			{
				throw std::runtime_error(R"ex(TaskEdge::getCursor is not implemented)ex");
			}
		}

		inline void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::TaskEdgeHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		inline void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::TaskEdgeHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	TaskEdge(std::unique_ptr<const Concept>&& pimpl) noexcept;

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	inline TaskEdge(std::shared_ptr<T> pimpl) noexcept
		: TaskEdge { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	[[nodiscard]] static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(TaskEdge)gql" };
	}
};

} // namespace graphql::today::object

#endif // TASKEDGEOBJECT_H
