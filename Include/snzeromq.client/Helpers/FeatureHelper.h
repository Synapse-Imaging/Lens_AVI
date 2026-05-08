#pragma once
#include <string>
#include <memory>
#include <cstdio>
#include <cppzmq/zmq.hpp>
#include <snzeromq.common/Models/Enums/FeatureType.h>

namespace synapse::network::cpp::zeromq::client::service::helpers
{
	class FeatureHelper {
		public:
			static void FindAddress(const char* info, char* find_info);
			static std::string FindAddress(const std::string& info);

			static void ClearMessageQueue(zmq::socket_t& socket);
			static void ClearMessageQueue(std::unique_ptr<zmq::socket_t>& socket);

			template <typename EnumType>
			static char* EnumToString(EnumType value);

			static char* EnumToString(common::enums::FeatureType value);
	};

    template<typename EnumType>
    inline char* FeatureHelper::EnumToString(EnumType value)
    {
        static_assert(std::is_enum<EnumType>::value, "EnumToString can only be used with enum types.");

        // 스레드 로컬 버퍼 (32바이트)
        thread_local char buffer[32];

        // 열거형 값의 기본 타입 추출
        using UnderlyingType = typename std::underlying_type<EnumType>::type;

        // 정수 값을 문자열로 변환하여 버퍼에 저장
        snprintf(buffer, sizeof(buffer), "%d", static_cast<UnderlyingType>(value));

        // 버퍼 반환
        return buffer;
    }
}
