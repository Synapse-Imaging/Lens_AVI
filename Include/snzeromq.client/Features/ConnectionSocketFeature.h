#pragma once
#include "FeatureBase.h"
#include <snzeromq.common/Models/Update/RequestTokenUpdate.h>
#include <snzeromq.common/Models/Update/ResponseTokenUpdate.h>

namespace synapse::network::cpp::zeromq::client::service::features
{
	class ConnectionSocketFeature : public FeatureBase
	{
	public:
		ConnectionSocketFeature(
			std::shared_ptr<common::helpers::JwtHelper> jwt_helper,
			std::string server_address, 
			int recv_timeout = 1000,
			int send_timeout = 1000);
		~ConnectionSocketFeature();
		ConnectionSocketFeature(const ConnectionSocketFeature& other) = default; // Copy constructor
		ConnectionSocketFeature& operator=(const ConnectionSocketFeature& other) = default; // Copy assignment
		ConnectionSocketFeature(ConnectionSocketFeature&& other) noexcept; // Move constructor
		ConnectionSocketFeature& operator=(ConnectionSocketFeature&& other) noexcept; // Move assignment

		std::optional<std::string> Initialize() override;

		bool Loopback(
			const common::models::SNZeroMQLoopback& request,
			common::models::SNZeroMQLoopback& response
		) override;

		bool UpdateToken(common::models::update::ResponseTokenUpdate& response);

		bool RequestConnection(
			const common::models::entry::RequestConnection& request,
			common::models::entry::ResponseConnection& response
		);
	private:
		const common::enums::FeatureType _feature_type = common::enums::FeatureType::Connection;
		char* _feature_type_string;

		std::optional<std::string> _host_address;
	};
}