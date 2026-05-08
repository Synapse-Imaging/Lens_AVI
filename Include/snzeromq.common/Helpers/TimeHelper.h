#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <ctime>

namespace synapse::network::cpp::zeromq::common::helpers
{
    class TimeHelper
    {
    public:
        /**
         * @brief 시간 데이터를 ISO 8601 형식(UTC) 문자열로 직렬화합니다.
         * @param time_t 시간 값
         * @param buffer 출력 버퍼
         * @param buffer_size 출력 버퍼 크기
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static void SerializeTime(time_t time, char* buffer, size_t buffer_size);

        /**
         * @brief ISO 8601 형식 문자열을 time_t로 역직렬화합니다.
         * @param time_string ISO 8601 형식 시간 문자열 ("YYYY-MM-DDTHH:MM:SS.mmmZ")
         * @return 역직렬화된 time_t 값
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static time_t DeserializeTime(const char* time_string);

        /**
         * @brief 주어진 시간 값을 UTC 시간으로 변환하여 보정합니다.
         * @param time 보정할 시간 값
         * @return UTC 기준으로 보정된 time_t 값
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static time_t EnsureUTC(time_t time);
    };
}
