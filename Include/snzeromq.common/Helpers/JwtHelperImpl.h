#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace synapse::network::cpp::zeromq::common::helpers {

    /**
     * @class JwtHelperImpl
     * @brief JWT (JSON Web Token) 구현 클래스.
     *
     * - JWT 토큰 생성 및 검증을 위한 기능을 제공합니다.
     * - HS256 알고리즘을 사용해 서명(Signature)을 생성 및 검증합니다.
     */
    class JwtHelperImpl {
    public:
        /**
         * @brief JwtHelperImpl 생성자.
         *
         * @param secret_key JWT 서명에 사용할 비밀 키.
         * @param key_id JWT 헤더에 포함될 Key ID (선택적).
         * @param expiry_margin_seconds 시간 검증 시 허용되는 여유 시간 (초 단위).
         */
        JwtHelperImpl(const std::string& secret_key, const std::string& key_id, int expiry_margin_seconds = 0);

        /**
         * @brief JWT 토큰을 생성합니다.
         *
         * @param expire_minutes 토큰의 유효 시간 (분 단위).
         * @param additional_claims 토큰에 포함할 추가 클레임 (key-value 형태).
         * @return 생성된 JWT 문자열.
         */
        std::string GenerateToken(int expire_minutes, const std::map<std::string, std::string>& additional_claims) const;

        /**
         * @brief JWT 토큰을 검증합니다.
         *
         * @param token 검증할 JWT 문자열.
         * @param claims JWT 페이로드에 포함된 클레임을 저장할 Map 객체.
         * @return 토큰이 유효하면 true, 그렇지 않으면 false.
         */
        bool ValidateToken(const std::string& token, std::map<std::string, std::string>& claims) const;

    private:
        /**
         * @brief JWT 서명에 사용되는 비밀 키.
         */
        std::string secret_key_;

        /**
         * @brief JWT 헤더에 포함되는 Key ID.
         */
        std::string key_id_;

        /**
         * @brief 시간 검증 시 허용되는 여유 시간 (초 단위).
         */
        int expiry_margin_seconds_;

        /**
         * @brief HMAC 서명을 생성합니다.
         *
         * @param data 서명할 데이터.
         * @return 생성된 HMAC 서명.
         */
        std::string Sign(const std::string& data) const;

        /**
         * @brief 문자열을 Base64 URL-safe 형식으로 인코딩합니다.
         *
         * @param input 인코딩할 문자열.
         * @return Base64 URL-safe로 인코딩된 문자열.
         */
        std::string Base64UrlEncode(const std::string& input) const;

        /**
         * @brief Base64 URL-safe 형식을 디코딩합니다.
         *
         * @param input 디코딩할 문자열.
         * @return 디코딩된 문자열.
         */
        std::string Base64UrlDecode(const std::string& input) const;

        /**
         * @brief 문자열을 구분자로 분리합니다.
         *
         * @param str 분리할 문자열.
         * @param delimiter 구분자 문자.
         * @return 분리된 문자열의 벡터.
         */
        std::vector<std::string> Split(const std::string& str, char delimiter) const;

        /**
         * @brief 고유한 GUID를 생성합니다.
         *
         * @return 생성된 GUID 문자열.
         */
        std::string GenerateGuid() const;
    };

} // namespace synapse::network::cpp::zeromq::common::helpers
