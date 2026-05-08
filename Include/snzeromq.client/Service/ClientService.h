#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API __declspec(dllimport)
#endif

#include <cppzmq/zmq.hpp>
#include <optional>
#include <vector>
#include <list>
#include <utility>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <map>

#include <snzeromq.common/Models/Enums/FeatureType.h>

#include <snzeromq.common/Data/PointListCollection.h>

#include <snzeromq.common/Models/ResponseServer.h>
#include <snzeromq.common/Models/SNZeroMQLoopback.h>

#include <snzeromq.common/Models/Enums/FeatureType.h>
#include <snzeromq.common/Models/Enums/ImageSearchMethod.h>
#include <snzeromq.common/Models/Enums/MessageType.h>
#include <snzeromq.common/Models/Enums/Sam2DataType.h>

#include <snzeromq.common/Models/Entry/ConnectionInfo.h>
#include <snzeromq.common/Models/Entry/RequestConnection.h>
#include <snzeromq.common/Models/Entry/ResponseConnection.h>

#include <snzeromq.common/Models/SAM2/RequestSam2.h>
#include <snzeromq.common/Models/SAM2/ResponseSam2.h>

#include <snzeromq.common/Models/Update/RequestTokenUpdate.h>
#include <snzeromq.common/Models/Update/ResponseTokenUpdate.h>

#include <snzeromq.common/Helpers/JwtHelper.h>
#include <snzeromq.common/Helpers/TimeHelper.h>

#include <snzeromq.client/Features/ConnectionSocketFeature.h>
#include <snzeromq.client/Features/SAM2SocketFeature.h>

namespace synapse::network::cpp::zeromq::client::service 
{
	class ClientService
	{
	public:
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ClientService();
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ~ClientService();
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ClientService(const ClientService& other) = default; // Copy constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ClientService& operator=(const ClientService& other) = default; // Copy assignment
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ClientService(ClientService&& other) noexcept; // Move constructor
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ClientService& operator=(ClientService&& other) noexcept; // Move assignment

		// Getter 
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* GetID() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* GetServerAddress() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* GetManagedConnectionServerAddress() const noexcept;
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* GetManagedSAM2ServerAddress() const noexcept;
		
		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* Initialize(
			const char* server_address, 
			int connection_recv_timeout = 1000,
			int connection_send_timeout = 1000);

		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool Loopback(
			const common::enums::FeatureType featureType,
			const common::models::SNZeroMQLoopback& send_loopback,
			common::models::SNZeroMQLoopback& receive_loopback);

		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool RequestFeatureInfo(
			const common::models::entry::RequestConnection& request_connection,
			common::models::entry::ResponseConnection& response_connection);

		SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool BindingFeature(
			const common::models::entry::ConnectionInfo& connection_info,
			int feature_recv_timeout = 5000, 
			int feature_send_timeout = 5000);



	private:
		// Setter
		void SetID(const char* v);
		void SetServerAddress(const char* v);
		void SetManagedConnectionServerAddress(const char* v);
		void SetManagedSAM2ServerAddress(const char* v);

		// Free
		void FreeID();
		void FreeServerAddress();
		void FreeManagedConnectionServerAddress();
		void FreeManagedSAM2ServerAddress();

		bool FindAddress(const char* info, char* find_info);

	private:
		char* _id = nullptr;
		char* _server_address = nullptr;
		char* _managed_connection_server_address = nullptr;
		char* _managed_sam2_server_address = nullptr;

		bool _is_initialize = false;
		std::map<common::enums::FeatureType, bool> _features_initialize;

		std::unique_ptr<features::ConnectionSocketFeature> _connection_socket;
		std::unique_ptr<features::SAM2SocketFeature> _sam2_socket;
		std::shared_ptr<common::helpers::JwtHelper> _jwt_helper;
	};
}