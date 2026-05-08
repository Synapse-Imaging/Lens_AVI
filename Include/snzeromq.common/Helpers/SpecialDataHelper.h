#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <vector>
#include <cstdint> // for uint8_t
#include <cstring> // for memcpy
#include <list>
#include "../Data/PointListCollection.h"

namespace synapse::network::cpp::zeromq::common::helpers {

    /**
     * @class SpecialDataHelper
     * @brief 점(Point) 및 점 리스트를 직렬화/역직렬화하는 헬퍼 클래스.
     *
     * - `PointListCollection`과 `std::list<std::list<std::pair<int, int>>>` 형식 간 변환을 지원합니다.
     * - 동적 메모리 관리 및 데이터 변환 기능을 포함합니다.
     */
    class SpecialDataHelper {
    public:
        /**
         * @brief PointListCollection을 직렬화합니다.
         *
         * @param data 직렬화할 PointListCollection 객체.
         * @return 직렬화된 데이터가 포함된 바이트 벡터.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static std::vector<uint8_t> SerializePoints(const data::PointListCollection& data);

        /**
         * @brief 바이트 벡터를 PointListCollection으로 역직렬화합니다.
         *
         * @param buffer 역직렬화할 바이트 벡터.
         * @return 역직렬화된 PointListCollection 객체.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static data::PointListCollection DeserializePoints(const std::vector<uint8_t>& buffer);

        /**
         * @brief std::list<std::list<std::pair<int, int>>>를 직렬화합니다.
         *
         * @param data 직렬화할 중첩 리스트 객체.
         * @return 직렬화된 데이터가 포함된 바이트 벡터.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static std::vector<uint8_t> SerializePoints(const std::list<std::list<std::pair<int, int>>>& data);

        /**
         * @brief 바이트 벡터를 std::list<std::list<std::pair<int, int>>>로 역직렬화합니다.
         *
         * @param buffer 역직렬화할 바이트 벡터.
         * @return 역직렬화된 중첩 리스트 객체.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static std::list<std::list<std::pair<int, int>>> UerStdDeserializePoints(const std::vector<uint8_t>& buffer);

        /**
         * @brief std::list<std::list<std::pair<int, int>>>를 PointListCollection으로 변환합니다.
         *
         * @param data 변환할 중첩 리스트 객체.
         * @return 변환된 PointListCollection 객체.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static synapse::network::cpp::zeromq::common::data::PointListCollection ConvertToPointListCollection(
            const std::list<std::list<std::pair<int, int>>>& data);

        /**
         * @brief 동적 할당된 PointListCollection의 메모리를 해제합니다.
         *
         * @param collection 해제할 PointListCollection 객체.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static void FreePointListCollection(synapse::network::cpp::zeromq::common::data::PointListCollection& collection);
    };
}
