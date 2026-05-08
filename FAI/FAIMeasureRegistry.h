#pragma once
#include "FAIMeasureSpec.h"
#include <map>
/**
 * 아직 개발 중... - 260213, jhkim
 */

/**
 * @file FAIMeasureRegistry.h
 * @brief FAI 측정 설정 레지스트리
 * @description 모든 FAI 측정 설정을 관리하는 중앙 집중식 레지스트리
 */

class FAIMeasureRegistry

{
public:
	static FAIMeasureRegistry& GetInstance()
	{
		static FAIMeasureRegistry instance;
		return instance;
	}

	/**
	 * @brief FAI 측정 설정 등록
	 */
	bool RegisterConfig(int faiId, const FAIMeasureSpec& config)
	{
		if (faiId < 0)
			return false;

		m_configs[faiId] = config;
		return true;
	}

	/**
	 * @brief FAI 측정 설정 조회
	 */
	const FAIMeasureSpec* GetConfig(int faiId) const
	{
		auto it = m_configs.find(faiId);
		if (it != m_configs.end())
		{
			// 조회후 Config 리턴.
			return &it->second;
		}

		return nullptr;
	}

	/**
	 * @brief 모든 등록된 FAI ID 반환
	 */
	void GetAllRegisteredIds(std::vector<int>& ids) const
	{
		ids.clear();
		
		for (const auto& pair : m_configs)
			ids.push_back(pair.first);
	}

	/**
	 * @brief 레지스트리 초기화
	 */
	void Clear()
	{
		m_configs.clear();
	}

	/**
	 * @brief 등록된 FAI 개수
	 */
	size_t GetConfigCount() const
	{
		return m_configs.size();
	}

private:
	FAIMeasureRegistry() = default;
	~FAIMeasureRegistry() = default;

	// 복사 방지
	FAIMeasureRegistry(const FAIMeasureRegistry&) = delete;
	FAIMeasureRegistry& operator=(const FAIMeasureRegistry&) = delete;

	std::map<int, FAIMeasureSpec> m_configs;
};
