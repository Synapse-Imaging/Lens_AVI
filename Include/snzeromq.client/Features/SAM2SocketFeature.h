#pragma once
#include "FeatureBase.h"
#include <snzeromq.common/Models/SAM2/RequestSam2.h>
#include <snzeromq.common/Models/SAM2/ResponseSam2.h>

namespace synapse::network::cpp::zeromq::client::service::features
{
	class SAM2SocketFeature : public FeatureBase
	{
	public:
		SAM2SocketFeature(
			std::shared_ptr<common::helpers::JwtHelper> jwt_helper,
			std::string server_address,
			int recv_timeout = 1000,
			int send_timeout = 1000);

		std::optional<std::string> Initialize() override;

		bool Loopback(
			const common::models::SNZeroMQLoopback& request,
			common::models::SNZeroMQLoopback& response
		) override;

		bool RequestSAM2(
			const common::models::sam2::RequestSam2& request,
			const uint8_t* request_data,
			size_t request_data_size,
			common::models::sam2::ResponseSam2& response,
			uint8_t*& response_data,
			size_t& response_data_size
		);

	private:
		const common::enums::FeatureType _feature_type = common::enums::FeatureType::SAM2;
		char* _feature_type_string;
	};
}