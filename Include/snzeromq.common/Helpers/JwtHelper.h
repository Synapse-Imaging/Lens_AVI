#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include "JwtHelperImpl.h"
#include "../Data/JwtClaims.h"

namespace synapse::network::cpp::zeromq::common::helpers {

    /**
     * @class JwtHelper
     * @brief JWT (JSON Web Token) 헬퍼 클래스.
     *
     * - JWT 토큰 생성 및 검증을 위한 고수준 인터페이스를 제공합니다.
     * - 내부적으로 JwtHelperImpl 클래스를 사용하여 구현됩니다.
     */
    class JwtHelper {
    public:
        /**
         * @brief JwtHelper 생성자.
         *
         * @param secret_key JWT 서명에 사용할 비밀 키.
         * @param key_id JWT 헤더에 포함될 Key ID (선택적).
         * @param expiry_margin_seconds 시간 검증 시 허용되는 여유 시간 (초 단위).
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API JwtHelper(const char* secret_key, const char* key_id, int expiry_margin_seconds);

        /**
         * @brief JwtHelper 소멸자.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~JwtHelper();

        /**
         * @brief JWT 토큰을 생성합니다.
         *
         * @param expire_minutes 토큰의 유효 시간 (분 단위, 기본값: 30).
         * @param claims 토큰에 포함할 클레임 배열 (선택적).
         * @param claims_count 클레임 배열의 크기.
         * @return 생성된 JWT 문자열.
         * @note 반환된 문자열은 내부적으로 관리되며, 사용자에 의해 해제될 필요가 없습니다.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GenerateToken(int expire_minutes = 30, const data::JwtClaims* claims = nullptr, int claims_count = 0);

        /**
         * @brief JWT 토큰을 검증합니다.
         *
         * @param token 검증할 JWT 문자열.
         * @param claims JWT 페이로드에 포함된 클레임을 저장할 배열 (선택적).
         * @param claims_count 클레임 배열의 크기. 검증 후 실제 클레임 수로 업데이트됩니다.
         * @return 토큰이 유효하면 true, 그렇지 않으면 false.
         */
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API bool ValidateToken(const char* token, data::JwtClaims* claims, int& claims_count);

    private:
        /**
         * @brief JWT 내부 구현 클래스.
         *
         * JwtHelperImpl 클래스의 인스턴스를 관리하여 실제 구현 로직을 처리합니다.
         */
        JwtHelperImpl* impl_;
    };

} // namespace synapse::network::cpp::zeromq::common::helpers
