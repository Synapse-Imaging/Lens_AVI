#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <string.h>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../ResponseServer.h"

namespace synapse::network::cpp::zeromq::common::models::sam2
{
	class ResponseSam2 : public models::ResponseServer
	{
	public:
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseSam2();
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseSam2(const char* id, bool result, const char* message, const char* reason);

		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseSam2 FromJson(const nlohmann::json& j);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseSam2 FromString(const char* str);
	};
}