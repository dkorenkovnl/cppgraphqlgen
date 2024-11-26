// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef TELEMETRYOBJECT_H
#define TELEMETRYOBJECT_H

#include "TestServerSchema.h"

namespace graphql::test_server::object {
namespace methods::TelemetryHas {

template <class TImpl>
concept getIdWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<response::IdType> { impl.getId(std::move(params)) } };
};

template <class TImpl>
concept getId = requires (TImpl impl)
{
	{ service::AwaitableScalar<response::IdType> { impl.getId() } };
};

template <class TImpl>
concept getWhenWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<response::Value> { impl.getWhen(std::move(params)) } };
};

template <class TImpl>
concept getWhen = requires (TImpl impl)
{
	{ service::AwaitableScalar<response::Value> { impl.getWhen() } };
};

template <class TImpl>
concept getNameWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<std::string> { impl.getName(std::move(params)) } };
};

template <class TImpl>
concept getName = requires (TImpl impl)
{
	{ service::AwaitableScalar<std::string> { impl.getName() } };
};

template <class TImpl>
concept getSensorIdWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<int> { impl.getSensorId(std::move(params)) } };
};

template <class TImpl>
concept getSensorId = requires (TImpl impl)
{
	{ service::AwaitableScalar<int> { impl.getSensorId() } };
};

template <class TImpl>
concept getSensorValueWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<double> { impl.getSensorValue(std::move(params)) } };
};

template <class TImpl>
concept getSensorValue = requires (TImpl impl)
{
	{ service::AwaitableScalar<double> { impl.getSensorValue() } };
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

} // namespace methods::TelemetryHas

class [[nodiscard("unnecessary construction")]] Telemetry final
	: public service::Object
{
private:
	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolveId(service::ResolverParams&& params) const;
	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolveWhen(service::ResolverParams&& params) const;
	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolveSensorId(service::ResolverParams&& params) const;
	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolveSensorValue(service::ResolverParams&& params) const;

	[[nodiscard("unnecessary call")]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard("unnecessary construction")]] Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		[[nodiscard("unnecessary call")]] virtual service::AwaitableScalar<response::IdType> getId(service::FieldParams&& params) const = 0;
		[[nodiscard("unnecessary call")]] virtual service::AwaitableScalar<response::Value> getWhen(service::FieldParams&& params) const = 0;
		[[nodiscard("unnecessary call")]] virtual service::AwaitableScalar<std::string> getName(service::FieldParams&& params) const = 0;
		[[nodiscard("unnecessary call")]] virtual service::AwaitableScalar<int> getSensorId(service::FieldParams&& params) const = 0;
		[[nodiscard("unnecessary call")]] virtual service::AwaitableScalar<double> getSensorValue(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct [[nodiscard("unnecessary construction")]] Model final
		: Concept
	{
		explicit Model(std::shared_ptr<T> pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard("unnecessary call")]] service::AwaitableScalar<response::IdType> getId(service::FieldParams&& params) const override
		{
			if constexpr (methods::TelemetryHas::getIdWithParams<T>)
			{
				return { _pimpl->getId(std::move(params)) };
			}
			else if constexpr (methods::TelemetryHas::getId<T>)
			{
				return { _pimpl->getId() };
			}
			else
			{
				throw service::unimplemented_method(R"ex(Telemetry::getId)ex");
			}
		}

		[[nodiscard("unnecessary call")]] service::AwaitableScalar<response::Value> getWhen(service::FieldParams&& params) const override
		{
			if constexpr (methods::TelemetryHas::getWhenWithParams<T>)
			{
				return { _pimpl->getWhen(std::move(params)) };
			}
			else if constexpr (methods::TelemetryHas::getWhen<T>)
			{
				return { _pimpl->getWhen() };
			}
			else
			{
				throw service::unimplemented_method(R"ex(Telemetry::getWhen)ex");
			}
		}

		[[nodiscard("unnecessary call")]] service::AwaitableScalar<std::string> getName(service::FieldParams&& params) const override
		{
			if constexpr (methods::TelemetryHas::getNameWithParams<T>)
			{
				return { _pimpl->getName(std::move(params)) };
			}
			else if constexpr (methods::TelemetryHas::getName<T>)
			{
				return { _pimpl->getName() };
			}
			else
			{
				throw service::unimplemented_method(R"ex(Telemetry::getName)ex");
			}
		}

		[[nodiscard("unnecessary call")]] service::AwaitableScalar<int> getSensorId(service::FieldParams&& params) const override
		{
			if constexpr (methods::TelemetryHas::getSensorIdWithParams<T>)
			{
				return { _pimpl->getSensorId(std::move(params)) };
			}
			else if constexpr (methods::TelemetryHas::getSensorId<T>)
			{
				return { _pimpl->getSensorId() };
			}
			else
			{
				throw service::unimplemented_method(R"ex(Telemetry::getSensorId)ex");
			}
		}

		[[nodiscard("unnecessary call")]] service::AwaitableScalar<double> getSensorValue(service::FieldParams&& params) const override
		{
			if constexpr (methods::TelemetryHas::getSensorValueWithParams<T>)
			{
				return { _pimpl->getSensorValue(std::move(params)) };
			}
			else if constexpr (methods::TelemetryHas::getSensorValue<T>)
			{
				return { _pimpl->getSensorValue() };
			}
			else
			{
				throw service::unimplemented_method(R"ex(Telemetry::getSensorValue)ex");
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const override
		{
			if constexpr (methods::TelemetryHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const override
		{
			if constexpr (methods::TelemetryHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	explicit Telemetry(std::unique_ptr<const Concept> pimpl) noexcept;

	[[nodiscard("unnecessary call")]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard("unnecessary call")]] service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const override;
	void endSelectionSet(const service::SelectionSetParams& params) const override;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	explicit Telemetry(std::shared_ptr<T> pimpl) noexcept
		: Telemetry { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	[[nodiscard("unnecessary call")]] static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(Telemetry)gql" };
	}
};

} // namespace graphql::test_server::object

#endif // TELEMETRYOBJECT_H
