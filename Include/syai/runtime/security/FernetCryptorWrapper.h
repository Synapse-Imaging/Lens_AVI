#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../SimpleSmartType.h"

#define SYAIRUNTIMEINSPECTION_EXPORTS
#ifdef SYAIRUNTIMEINSPECTION_EXPORTS
#define SYAIRUNTIMEINSPECTION_API __declspec(dllexport)
#else
#define SYAIRUNTIMEINSPECTION_API __declspec(dllimport)
#endif

namespace syai::runtime::security
{
	/**
	 * @brief Fernet 암호화를 위한 래퍼 클래스
	 *
	 * syai::security::encryption::FernetCryptor를 syai_runtime_inspection을 통해
	 * 사용할 수 있도록 래핑한 클래스입니다.
	 *
	 * syai_runtime_shared 프로젝트의 FernetCryptor는 shared 프로젝트이므로
	 * SYAI.Runtime.Demo에서 직접 사용할 수 없지만, 이 래퍼 클래스를 통해
	 * syai_runtime_inspection (라이브러리 프로젝트)의 인터페이스로 접근 가능합니다.
	 *
	 * 특징:
	 * - Opaque Pointer 패턴으로 구현 은닉
	 * - SmartString을 사용한 안전한 문자열 관리
	 * - 텍스트 및 바이너리 토큰 모두 지원
	 * - 암호화/복호화 기능 제공
	 *
	 * 사용 예:
	 * @code
	 *     FernetCryptorWrapper cryptor;
	 *     std::string encrypted = cryptor.encrypt_to_text("my secret data");
	 *     std::string decrypted = cryptor.decrypt_text(encrypted);
	 * @endcode
	 *
	 * @note Fernet 키는 "base64url로 인코딩된 32 bytes" 형식이어야 합니다.
	 */
	class SYAIRUNTIMEINSPECTION_API FernetCryptorWrapper
	{
	public:
		/**
		 * @brief FernetCryptor 생성자
		 * @param fernet_key_b64url base64url로 인코딩된 Fernet 키 (32 bytes 디코딩)
		 * @throws std::runtime_error 키가 유효하지 않으면 발생
		 */
		explicit FernetCryptorWrapper(const SmartString& fernet_key_b64url);

		/**
		 * @brief FernetCryptor 생성자
		 * @param fernet_key_b64url base64url로 인코딩된 Fernet 키 (C 문자열)
		 * @throws std::runtime_error 키가 유효하지 않으면 발생
		 */
		explicit FernetCryptorWrapper(const char* fernet_key_b64url);

		// 복사 생성자 및 할당 연산자 삭제 (Opaque 패턴)
		FernetCryptorWrapper(const FernetCryptorWrapper&) = delete;
		FernetCryptorWrapper& operator=(const FernetCryptorWrapper&) = delete;

		// 이동 생성자 및 할당 연산자
		FernetCryptorWrapper(FernetCryptorWrapper&&) noexcept;
		FernetCryptorWrapper& operator=(FernetCryptorWrapper&&) noexcept;

		/**
		 * @brief 소멸자
		 */
		~FernetCryptorWrapper();

		/**
		 * @brief 평문 데이터를 암호화하여 base64url 텍스트 토큰으로 변환
		 *
		 * @param plain_text 암호화할 평문 (C 문자열)
		 * @return base64url로 인코딩된 암호화 토큰 (std::string)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 *
		 * @code
		 *     FernetCryptorWrapper cryptor(key);
		 *     std::string token = cryptor.encrypt_to_text("secret data");
		 * @endcode
		 */
		std::string encrypt_to_text(const char* plain_text) const;

		/**
		 * @brief 평문 데이터를 암호화하여 base64url 텍스트 토큰으로 변환
		 *
		 * @param plain_text 암호화할 평문 (SmartString)
		 * @return base64url로 인코딩된 암호화 토큰 (std::string)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 */
		std::string encrypt_to_text(const SmartString& plain_text) const;

		/**
		 * @brief 바이너리 데이터를 암호화하여 base64url 텍스트 토큰으로 변환
		 *
		 * @param plain 암호화할 바이너리 데이터
		 * @param plain_len 데이터 길이 (바이트)
		 * @return base64url로 인코딩된 암호화 토큰 (std::string)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 */
		std::string encrypt_binary_to_text(const unsigned char* plain, size_t plain_len) const;

		/**
		 * @brief 평문 데이터를 암호화하여 바이너리 토큰으로 변환
		 *
		 * @param plain_text 암호화할 평문 (C 문자열)
		 * @return 바이너리 토큰 (std::vector<unsigned char>)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 */
		std::vector<unsigned char> encrypt_to_binary(const char* plain_text) const;

		/**
		 * @brief 평문 데이터를 암호화하여 바이너리 토큰으로 변환
		 *
		 * @param plain_text 암호화할 평문 (SmartString)
		 * @return 바이너리 토큰 (std::vector<unsigned char>)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 */
		std::vector<unsigned char> encrypt_to_binary(const SmartString& plain_text) const;

		/**
		 * @brief 바이너리 데이터를 암호화하여 바이너리 토큰으로 변환
		 *
		 * @param plain 암호화할 바이너리 데이터
		 * @param plain_len 데이터 길이 (바이트)
		 * @return 바이너리 토큰 (std::vector<unsigned char>)
		 * @throws std::runtime_error 암호화 실패 시 발생
		 */
		std::vector<unsigned char> encrypt_binary_to_binary(const unsigned char* plain, size_t plain_len) const;

		/**
		 * @brief base64url 텍스트 토큰을 복호화하여 평문으로 변환
		 *
		 * @param token_b64url base64url로 인코딩된 토큰 (C 문자열)
		 * @return 복호화된 평문 (std::string)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 *
		 * @code
		 *     FernetCryptorWrapper cryptor(key);
		 *     std::string encrypted = cryptor.encrypt_to_text("secret");
		 *     std::string decrypted = cryptor.decrypt_text(encrypted);
		 *     // decrypted == "secret"
		 * @endcode
		 */
		std::string decrypt_text(const char* token_b64url) const;

		/**
		 * @brief base64url 텍스트 토큰을 복호화하여 평문으로 변환
		 *
		 * @param token_b64url base64url로 인코딩된 토큰 (SmartString)
		 * @return 복호화된 평문 (std::string)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 */
		std::string decrypt_text(const SmartString& token_b64url) const;

		/**
		 * @brief base64url 텍스트 토큰을 복호화하여 바이너리로 변환
		 *
		 * @param token_b64url base64url로 인코딩된 토큰 (C 문자열)
		 * @return 복호화된 바이너리 데이터 (std::vector<unsigned char>)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 */
		std::vector<unsigned char> decrypt_text_to_binary(const char* token_b64url) const;

		/**
		 * @brief base64url 텍스트 토큰을 복호화하여 바이너리로 변환
		 *
		 * @param token_b64url base64url로 인코딩된 토큰 (SmartString)
		 * @return 복호화된 바이너리 데이터 (std::vector<unsigned char>)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 */
		std::vector<unsigned char> decrypt_text_to_binary(const SmartString& token_b64url) const;

		/**
		 * @brief 바이너리 토큰을 복호화하여 평문으로 변환
		 *
		 * @param token_bytes 바이너리 토큰 데이터
		 * @param token_len 토큰 길이 (바이트)
		 * @return 복호화된 평문 (std::string)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 */
		std::string decrypt_binary(const unsigned char* token_bytes, size_t token_len) const;

		/**
		 * @brief 바이너리 토큰을 복호화하여 바이너리로 변환
		 *
		 * @param token_bytes 바이너리 토큰 데이터
		 * @param token_len 토큰 길이 (바이트)
		 * @return 복호화된 바이너리 데이터 (std::vector<unsigned char>)
		 * @throws std::runtime_error 복호화 실패 시 발생
		 */
		std::vector<unsigned char> decrypt_binary_to_binary(const unsigned char* token_bytes, size_t token_len) const;

		/**
		 * @brief 래퍼가 올바르게 초기화되었는지 확인
		 *
		 * @return 초기화되었으면 true, 그렇지 않으면 false
		 */
		bool is_initialized() const noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> pImpl_;
	};

} // namespace syai::runtime::security

