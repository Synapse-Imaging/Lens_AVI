#pragma once
#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include "../ResponseServer.h"

namespace synapse::network::cpp::zeromq::common::models::update
{
	class ResponseTokenUpdate : public ResponseServer
	{
	public:
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate();
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate(const char* id, bool result, const char* message, const char* reason, const char* update_token);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~ResponseTokenUpdate();// Destructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate(const ResponseTokenUpdate& other); // Copy constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate& operator=(const ResponseTokenUpdate& other); // Copy assignment operator
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate(ResponseTokenUpdate&& other) noexcept; // Move constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseTokenUpdate& operator=(ResponseTokenUpdate&& other) noexcept; // Move assignment operator
		
		// UpdateToken Getter and Setter
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetUpdateToken() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SerUpdateToken(const char* v);

		// JSON Serialization
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseTokenUpdate FromJson(const nlohmann::json& j);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseTokenUpdate FromString(const char* str);
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API virtual nlohmann::json ToJson() const;

		// Convert to String
		SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const override;

	private:
		char* _update_token = nullptr;

		void FreeUpdateToken();

		struct Keys : ResponseServer::Keys
		{
			static constexpr const char* UpdateToken = "update_token";
		};
	};
}