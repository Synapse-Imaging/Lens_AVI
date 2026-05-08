#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include "../Enums/Sam2DataType.h"
#include <cstring>

namespace synapse::network::cpp::zeromq::common::models::update
{
	class RequestTokenUpdate
	{
	public:
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate();
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate(const char* old_token, const char* new_token);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~RequestTokenUpdate(); // Destructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate(const RequestTokenUpdate& other);// Copy constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate& operator=(const RequestTokenUpdate& other); // Copy assignment operator
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate(RequestTokenUpdate&& other) noexcept; // Move constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestTokenUpdate& operator=(RequestTokenUpdate&& other) noexcept; // Move assignment operator

		// OldToken Getter and Setter
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetOldToken() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetOldToken(const char* v);

		// NewToken Getter and Setter
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetNewToken() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetNewToken(const char* v);

		// JSON Serialization
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static bool FromJson(const nlohmann::json& j, RequestTokenUpdate& obj);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static bool FromString(const char* str, RequestTokenUpdate& obj);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API nlohmann::json ToJson() const;

		// Convert to String
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const;

	private:
		char* _old_token = nullptr;
		char* _new_token = nullptr;

		void FreeOldToken();
		void FreeNewToken();

		struct Keys {
			static constexpr const char* OldToken = "old_token";
			static constexpr const char* NewToken = "new_token";
		};
	};
}