#pragma once
namespace synapse::network::cpp::zeromq::common::enums {
	enum class FeatureType
	{
		Connection,
		Info,
		Data,
		SAM2,
		None = 999,
	};
}