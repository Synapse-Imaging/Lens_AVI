#pragma once
#include <intrin.h>
#include "syai_runtime_inspection_export.h"

namespace syai::runtime
{
/**
 * @brief 스마트 문자열 클래스 (참조 카운팅 기반 자동 메모리 관리)
 *
 * std::string과 유사하지만 표준 라이브러리를 사용하지 않으며,
 * 참조 카운팅을 통해 메모리를 자동으로 관리함.
 * 복사 시 실제 데이터를 복사하지 않고 참조만 증가시켜 성능을 최적화함.
 *
 * @note 멀티스레드 환경에서는 동기화 고려 필요
 * @note 문자열 수정 시 Copy-on-Write 패턴 미구현 (읽기 전용)
 *
 * 예제:
 * @code
 *     SmartString str1("Hello");
 *     SmartString str2 = str1;  // 참조 카운트 증가, 실제 복사 없음
 *     const char* c_str = str1.c_str();  // C 스타일 문자열 접근
 * @endcode
 */
class SYAIRUNTIMEINSPECTION_API SmartString
{
private:
struct StringData;
StringData* str_data_;

void release();
void add_ref();

public:
SmartString();
SmartString(const char* str);
SmartString(const SmartString& other);
SmartString& operator=(const SmartString& other);
SmartString& operator=(const char* str);
~SmartString();

const char* c_str() const;
bool empty() const;
int ref_count() const;
int length() const;

bool operator==(const SmartString& other) const;
bool operator!=(const SmartString& other) const;
bool operator<(const SmartString& other) const;
bool operator>(const SmartString& other) const;
bool operator<=(const SmartString& other) const;
bool operator>=(const SmartString& other) const;
};

/**
 * @brief 스마트 리스트 클래스 (참조 카운팅 기반 자동 메모리 관리)
 *
 * 문자열 배열을 참조 카운팅을 통해 자동으로 관리하는 클래스.
 * SmartString과 동일한 패턴을 사용하여 구현됨.
 *
 * 특징:
 * - 자동 메모리 관리: 참조 카운트가 0이 되면 자동으로 배열 삭제
 * - 복사 시 참조 카운팅: 실제 배열 복사 없이 참조만 공유
 * - 동적 크기 조정: 항목 추가/제거 시 자동으로 메모리 재할당
 *
 * @note 멀티스레드 환경에서는 동기화 고려 필요
 * @note 수정 시 Copy-on-Write 패턴 미구현 (직접 수정)
 *
 * 예제:
 * @code
 *     SmartList list;
 *     list.add("defect1");
 *     list.add("defect2");
 *
 *     SmartList copy = list;  // 참조 카운트 증가, 실제 복사 없음
 *     const char* item = list.get(0);  // "defect1"
 *
 *     for (int i = 0; i < list.size(); ++i) {
 *         const char* defect = list.get(i);
 *     }
 * @endcode
 */
class SYAIRUNTIMEINSPECTION_API SmartList
{
private:
struct ListData;
ListData* data_;

void release();
void add_ref();
void ensure_unique();
bool string_equals(const char* str1, const char* str2) const;

public:
SmartList();
explicit SmartList(int initial_capacity);
SmartList(const SmartList& other);
SmartList& operator=(const SmartList& other);
~SmartList();

void add(const char* str);
const char* get(int index) const;
void set(int index, const char* str);
void remove_at(int index);
bool remove(const char* str);
int index_of(const char* str) const;
bool contains(const char* str) const;
int size() const;
bool empty() const;
void clear();
int ref_count() const;

const char* operator[](int index) const;
bool operator==(const SmartList& other) const;
bool operator!=(const SmartList& other) const;
};


	/**
	 * @brief 제네릭 스마트 벡터 클래스 (참조 카운팅 기반 자동 메모리 관리)
	 *
	 * 임의의 타입 T를 참조 카운팅을 통해 자동으로 관리하는 벡터 클래스.
	 * SmartString과 동일한 참조 카운팅 패턴을 사용하여 구현됨.
	 *
	 * 특징:
	 * - 자동 메모리 관리: 참조 카운트가 0이 되면 자동으로 배열 삭제
	 * - 복사 시 참조 카운팅: 실제 배열 복사 없이 참조만 공유
	 * - 동적 크기 조정: 항목 추가/제거 시 자동으로 메모리 재할당
	 * - 타입 안전성: 컴파일 타임에 타입 검증
	 * - Copy-on-Write: 수정 시에만 데이터 복제
	 *
	 * @tparam T 관리할 데이터 타입
	 *
	 * @note 멀티스레드 환경에서는 동기화 고려 필요
	 * @note T 타입은 복사 가능해야 함
	 *
	 * 예제:
	 * @code
	 *     SmartVector<int> int_vec;
	 *     int_vec.push_back(1);
	 *     int_vec.push_back(2);
	 *
	 *     SmartVector<ManagerInfo> manager_vec;
	 *     manager_vec.push_back(ManagerInfo("test"));
	 *
	 *     SmartVector<int> copy = int_vec;  // 참조 카운트 증가, 실제 복사 없음
	 *     int value = int_vec.get(0);  // 1
	 * @endcode
	 */
	template<typename T>
	class SmartVector
	{
	private:
		/**
		 * @brief 벡터 데이터와 참조 카운트를 관리하는 내부 구조체
		 *
		 * 실제 데이터 배열, 참조 카운트, 배열 크기를 포함하며,
		 * 여러 SmartVector 인스턴스가 공유할 수 있음.
		 */
		struct VectorData
		{
			T* items;            //!< 데이터 배열
			int* ref_count;      //!< 참조 카운트 포인터
			int size;            //!< 현재 항목 개수
			int capacity;        //!< 할당된 배열 용량

			/**
			 * @brief VectorData 생성자
			 * @param initial_capacity 초기 배열 용량 (기본: 4)
			 */
			VectorData(int initial_capacity = 4)
				: items(nullptr), ref_count(nullptr), size(0), capacity(initial_capacity)
			{
				if (initial_capacity > 0) {
					items = new T[initial_capacity];
					ref_count = new int(1);
				}
			}

			/**
			 * @brief VectorData 소멸자
			 * @warning 참조 카운트가 0이 될 때만 호출되어야 함
			 */
			~VectorData()
			{
				if (items) {
					delete[] items;
				}
				if (ref_count) {
					delete ref_count;
				}
			}

			/**
			 * @brief 배열 용량 확장
			 * @param new_capacity 새로운 용량
			 */
			void resize(int new_capacity)
			{
				if (new_capacity <= capacity) return;

				T* new_items = new T[new_capacity];

				// 기존 항목들 이동 (move semantics 사용)
				for (int i = 0; i < size; ++i) {
					new_items[i] = static_cast<T&&>(items[i]);
				}

				delete[] items;
				items = new_items;
				capacity = new_capacity;
			}
		};

		VectorData* data_; //!< 공유 벡터 데이터 포인터

		/**
		 * @brief 참조 카운트 해제 및 필요시 메모리 정리
		 */
		void release()
		{
			if (data_ && data_->ref_count) {
				--(*data_->ref_count);
				if (*data_->ref_count == 0) {
					delete data_;
				}
				data_ = nullptr;
			}
		}

		/**
		 * @brief 참조 카운트 증가
		 */
		void add_ref()
		{
			if (data_ && data_->ref_count) {
				++(*data_->ref_count);
			}
		}

		/**
		 * @brief Copy-on-Write를 위한 데이터 복제
		 * @note 참조 카운트가 1보다 크면 데이터를 복제함
		 */
		void ensure_unique()
		{
			if (!data_ || !data_->ref_count || *data_->ref_count <= 1) {
				return;
			}

			// 새 데이터 생성
			VectorData* new_data = new VectorData(data_->capacity);
			new_data->size = data_->size;

			// 모든 항목 복사
			for (int i = 0; i < data_->size; ++i) {
				new_data->items[i] = data_->items[i];
			}

			// 기존 참조 해제 후 새 데이터 사용
			release();
			data_ = new_data;
		}

	public:
		//! 기본 생성자 (빈 벡터)
		SmartVector() : data_(nullptr) {}

		/**
		 * @brief 초기 용량으로 생성
		 * @param initial_capacity 초기 배열 용량
		 */
		explicit SmartVector(int initial_capacity) : data_(nullptr)
		{
			if (initial_capacity > 0) {
				data_ = new VectorData(initial_capacity);
			}
		}

		/**
		 * @brief 복사 생성자 (참조 카운팅)
		 * @param other 복사할 SmartVector 인스턴스
		 */
		SmartVector(const SmartVector& other) : data_(other.data_)
		{
			add_ref();
		}

		/**
		 * @brief 할당 연산자
		 * @param other 할당할 SmartVector 인스턴스
		 * @return 자기 자신 참조
		 */
		SmartVector& operator=(const SmartVector& other)
		{
			if (this != &other) {
				release();
				data_ = other.data_;
				add_ref();
			}
			return *this;
		}

		//! 소멸자 (자동 메모리 관리)
		~SmartVector()
		{
			release();
		}

		/**
		 * @brief 벡터 끝에 항목 추가
		 * @param item 추가할 항목
		 */
		void push_back(const T& item)
		{
			if (!data_) {
				data_ = new VectorData();
			}

			ensure_unique();

			// 용량 부족 시 확장 (2배로 증가)
			if (data_->size >= data_->capacity) {
				int new_capacity = data_->capacity * 2;
				if (new_capacity < 4) new_capacity = 4;
				data_->resize(new_capacity);
			}

			// 항목 추가
			data_->items[data_->size] = item;
			++data_->size;
		}

		/**
		 * @brief 벡터 끝에 항목 추가 (move semantics)
		 * @param item 추가할 항목 (이동됨)
		 */
		void push_back(T&& item)
		{
			if (!data_) {
				data_ = new VectorData();
			}

			ensure_unique();

			// 용량 부족 시 확장 (2배로 증가)
			if (data_->size >= data_->capacity) {
				int new_capacity = data_->capacity * 2;
				if (new_capacity < 4) new_capacity = 4;
				data_->resize(new_capacity);
			}

			// 항목 이동
			data_->items[data_->size] = static_cast<T&&>(item);
			++data_->size;
		}

		/**
		 * @brief 특정 인덱스의 항목 반환
		 * @param index 반환할 항목의 인덱스
		 * @return 항목 참조 (범위 밖이면 예외 발생 가능)
		 */
		const T& get(int index) const
		{
			if (!data_ || index < 0 || index >= data_->size) {
				static T default_value{};
				return default_value;
			}
			return data_->items[index];
		}

		/**
		 * @brief 특정 인덱스의 항목 반환 (수정 가능)
		 * @param index 반환할 항목의 인덱스
		 * @return 항목 참조 (범위 밖이면 예외 발생 가능)
		 */
		T& get(int index)
		{
			if (!data_ || index < 0 || index >= data_->size) {
				static T default_value{};
				return default_value;
			}

			ensure_unique();
			return data_->items[index];
		}

		/**
		 * @brief 특정 인덱스의 항목 설정
		 * @param index 설정할 항목의 인덱스
		 * @param item 새로운 항목
		 */
		void set(int index, const T& item)
		{
			if (!data_ || index < 0 || index >= data_->size) {
				return;
			}

			ensure_unique();
			data_->items[index] = item;
		}

		/**
		 * @brief 특정 인덱스의 항목 제거
		 * @param index 제거할 항목의 인덱스
		 */
		void remove_at(int index)
		{
			if (!data_ || index < 0 || index >= data_->size) {
				return;
			}

			ensure_unique();

			// 뒤의 항목들을 앞으로 이동
			for (int i = index; i < data_->size - 1; ++i) {
				data_->items[i] = static_cast<T&&>(data_->items[i + 1]);
			}

			--data_->size;
		}

		/**
		 * @brief 특정 값과 일치하는 첫 번째 항목 제거
		 * @param item 제거할 항목
		 * @return 제거되었으면 true, 찾을 수 없으면 false
		 */
		bool remove(const T& item)
		{
			int index = index_of(item);
			if (index >= 0) {
				remove_at(index);
				return true;
			}
			return false;
		}

		/**
		 * @brief 특정 값의 인덱스 검색
		 * @param item 검색할 항목
		 * @return 찾은 인덱스 (찾을 수 없으면 -1)
		 */
		int index_of(const T& item) const
		{
			if (!data_) return -1;

			for (int i = 0; i < data_->size; ++i) {
				if (data_->items[i] == item) {
					return i;
				}
			}
			return -1;
		}

		/**
		 * @brief 특정 값이 포함되어 있는지 확인
		 * @param item 검색할 항목
		 * @return 포함되어 있으면 true, 없으면 false
		 */
		bool contains(const T& item) const
		{
			return index_of(item) >= 0;
		}

		/**
		 * @brief 현재 항목 개수 반환
		 * @return 항목 개수
		 */
		int size() const
		{
			return data_ ? data_->size : 0;
		}

		/**
		 * @brief 벡터가 비어있는지 확인
		 * @return 비어있으면 true, 그렇지 않으면 false
		 */
		bool empty() const
		{
			return size() == 0;
		}

		/**
		 * @brief 모든 항목 제거
		 */
		void clear()
		{
			if (!data_) return;

			ensure_unique();
			data_->size = 0;
		}

		/**
		 * @brief 현재 참조 카운트 반환 (디버깅용)
		 * @return 참조 카운트 값 (유효하지 않으면 0)
		 */
		int ref_count() const
		{
			return data_ && data_->ref_count ? *data_->ref_count : 0;
		}

		/**
		 * @brief 배열 형태로 접근 연산자 (읽기 전용)
		 * @param index 접근할 항목의 인덱스
		 * @return 항목 참조
		 */
		const T& operator[](int index) const
		{
			return get(index);
		}

		/**
		 * @brief 배열 형태로 접근 연산자 (수정 가능)
		 * @param index 접근할 항목의 인덱스
		 * @return 항목 참조
		 */
		T& operator[](int index)
		{
			return get(index);
		}

		/**
		 * @brief 동등성 비교 연산자
		 * @param other 비교할 SmartVector 인스턴스
		 * @return 모든 항목이 같으면 true, 다르면 false
		 */
		bool operator==(const SmartVector& other) const
		{
			if (size() != other.size()) return false;

			for (int i = 0; i < size(); ++i) {
				if (!(get(i) == other.get(i))) {
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief 부등성 비교 연산자
		 * @param other 비교할 SmartVector 인스턴스
		 * @return 항목이 다르면 true, 같으면 false
		 */
		bool operator!=(const SmartVector& other) const
		{
			return !(*this == other);
		}

		// ========== STL 호환성을 위한 iterator 지원 ==========

		/**
		 * @brief 시작 반복자 반환
		 * @return 시작 반복자
		 */
		T* begin()
		{
			if (!data_ || data_->size == 0) return nullptr;
			ensure_unique();
			return data_->items;
		}

		/**
		 * @brief 시작 반복자 반환 (읽기 전용)
		 * @return 시작 반복자
		 */
		const T* begin() const
		{
			if (!data_ || data_->size == 0) return nullptr;
			return data_->items;
		}

		/**
		 * @brief 끝 반복자 반환
		 * @return 끝 반복자
		 */
		T* end()
		{
			if (!data_ || data_->size == 0) return nullptr;
			ensure_unique();
			return data_->items + data_->size;
		}

		/**
		 * @brief 끝 반복자 반환 (읽기 전용)
		 * @return 끝 반복자
		 */
		const T* end() const
		{
			if (!data_ || data_->size == 0) return nullptr;
			return data_->items + data_->size;
		}

		// ========== 타입 별칭 (STL 호환성) ==========
		using value_type = T;
		using size_type = int;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;
	};

	// DLL 경계를 넘는 SmartVector 인스턴스에 대한 명시적 템플릿 인스턴스화 선언
	extern template class SYAIRUNTIMEINSPECTION_API SmartVector<bool>;

	/**
	 * @brief 제네릭 스마트 맵 클래스 (참조 카운팅 기반 자동 메모리 관리)
	 *
	 * 키-값 쌍을 참조 카운팅을 통해 자동으로 관리하는 맵 클래스.
	 * std::map과 유사한 기능을 제공하며, SmartString과 동일한 참조 카운팅 패턴을 사용함.
	 *
	 * 특징:
	 * - 자동 메모리 관리: 참조 카운트가 0이 되면 자동으로 배열 삭제
	 * - 복사 시 참조 카운팅: 실제 배열 복사 없이 참조만 공유
	 * - 동적 크기 조정: 항목 추가/제거 시 자동으로 메모리 재할당
	 * - 타입 안전성: 컴파일 타임에 타입 검증
	 * - Copy-on-Write: 수정 시에만 데이터 복제
	 * - 정렬된 순서: 키를 기준으로 정렬된 상태 유지
	 *
	 * @tparam Key 키 타입 (비교 연산자 필요)
	 * @tparam Value 값 타입 (복사 가능해야 함)
	 *
	 * @note 멀티스레드 환경에서는 동기화 고려 필요
	 * @note Key 타입은 < 연산자와 == 연산자가 정의되어야 함
	 * @note Value 타입은 복사 가능해야 함
	 *
	 * 예제:
	 * @code
	 *     SmartMap<SmartString, int> score_map;
	 *     score_map.insert("player1", 100);
	 *     score_map.insert("player2", 85);
	 *     score_map["player3"] = 92;
	 *
	 *     SmartMap<SmartString, int> copy = score_map;  // 참조 카운트 증가
	 *     int score = score_map.at("player1");  // 100
	 *
	 *     if (score_map.contains("player2")) {
	 *         score_map.erase("player2");
	 *     }
	 * @endcode
	 */
	template<typename Key, typename Value>
	class SmartMap
	{
	private:
		/**
		 * @brief 키-값 쌍을 저장하는 구조체
		 */
		struct KeyValuePair
		{
			Key key;        //!< 키
			Value value;    //!< 값

			KeyValuePair() = default;
			KeyValuePair(const Key& k, const Value& v) : key(k), value(v) {}
			KeyValuePair(Key&& k, Value&& v) : key(static_cast<Key&&>(k)), value(static_cast<Value&&>(v)) {}
		};

		/**
		 * @brief 맵 데이터와 참조 카운트를 관리하는 내부 구조체
		 *
		 * 실제 키-값 쌍 배열, 참조 카운트, 배열 크기를 포함하며,
		 * 여러 SmartMap 인스턴스가 공유할 수 있음.
		 */
		struct MapData
		{
			KeyValuePair* pairs;    //!< 키-값 쌍 배열 (정렬된 상태 유지)
			int* ref_count;         //!< 참조 카운트 포인터
			int size;               //!< 현재 항목 개수
			int capacity;           //!< 할당된 배열 용량

			/**
			 * @brief MapData 생성자
			 * @param initial_capacity 초기 배열 용량 (기본: 4)
			 */
			MapData(int initial_capacity = 4)
				: pairs(nullptr), ref_count(nullptr), size(0), capacity(initial_capacity)
			{
				if (initial_capacity > 0) {
					pairs = new KeyValuePair[initial_capacity];
					ref_count = new int(1);
				}
			}

			/**
			 * @brief MapData 소멸자
			 * @warning 참조 카운트가 0이 될 때만 호출되어야 함
			 */
			~MapData()
			{
				if (pairs) {
					delete[] pairs;
				}
				if (ref_count) {
					delete ref_count;
				}
			}

			/**
			 * @brief 배열 용량 확장
			 * @param new_capacity 새로운 용량
			 */
			void resize(int new_capacity)
			{
				if (new_capacity <= capacity) return;

				KeyValuePair* new_pairs = new KeyValuePair[new_capacity];

				// 기존 항목들 이동
				for (int i = 0; i < size; ++i) {
					new_pairs[i] = static_cast<KeyValuePair&&>(pairs[i]);
				}

				delete[] pairs;
				pairs = new_pairs;
				capacity = new_capacity;
			}

			/**
			 * @brief 키에 대한 인덱스 찾기 (이진 탐색)
			 * @param key 찾을 키
			 * @return 키의 인덱스 (없으면 -1)
			 */
			int find_index(const Key& key) const
			{
				int left = 0;
				int right = size - 1;

				while (left <= right) {
					int mid = left + (right - left) / 2;

					if (pairs[mid].key == key) {
						return mid;
					}
					else if (pairs[mid].key < key) {
						left = mid + 1;
					}
					else {
						right = mid - 1;
					}
				}
				return -1;
			}

			/**
			 * @brief 키를 삽입할 위치 찾기 (이진 탐색)
			 * @param key 삽입할 키
			 * @return 삽입 위치 인덱스
			 */
			int find_insert_position(const Key& key) const
			{
				int left = 0;
				int right = size;

				while (left < right) {
					int mid = left + (right - left) / 2;

					if (pairs[mid].key < key) {
						left = mid + 1;
					}
					else {
						right = mid;
					}
				}
				return left;
			}
		};

		MapData* data_; //!< 공유 맵 데이터 포인터

		/**
		 * @brief 참조 카운트 해제 및 필요시 메모리 정리
		 */
		void release()
		{
			if (data_ && data_->ref_count) {
				--(*data_->ref_count);
				if (*data_->ref_count == 0) {
					delete data_;
				}
				data_ = nullptr;
			}
		}

		/**
		 * @brief 참조 카운트 증가
		 */
		void add_ref()
		{
			if (data_ && data_->ref_count) {
				++(*data_->ref_count);
			}
		}

		/**
		 * @brief Copy-on-Write를 위한 데이터 복제
		 * @note 참조 카운트가 1보다 크면 데이터를 복제함
		 */
		void ensure_unique()
		{
			if (!data_ || !data_->ref_count || *data_->ref_count <= 1) {
				return;
			}

			// 새 데이터 생성
			MapData* new_data = new MapData(data_->capacity);
			new_data->size = data_->size;

			// 모든 키-값 쌍 복사
			for (int i = 0; i < data_->size; ++i) {
				new_data->pairs[i] = data_->pairs[i];
			}

			// 기존 참조 해제 후 새 데이터 사용
			release();
			data_ = new_data;
		}

	public:
		//! 기본 생성자 (빈 맵)
		SmartMap() : data_(nullptr) {}

		/**
		 * @brief 초기 용량으로 생성
		 * @param initial_capacity 초기 배열 용량
		 */
		explicit SmartMap(int initial_capacity) : data_(nullptr)
		{
			if (initial_capacity > 0) {
				data_ = new MapData(initial_capacity);
			}
		}

		/**
		 * @brief 복사 생성자 (참조 카운팅)
		 * @param other 복사할 SmartMap 인스턴스
		 */
		SmartMap(const SmartMap& other) : data_(other.data_)
		{
			add_ref();
		}

		/**
		 * @brief 할당 연산자
		 * @param other 할당할 SmartMap 인스턴스
		 * @return 자기 자신 참조
		 */
		SmartMap& operator=(const SmartMap& other)
		{
			if (this != &other) {
				release();
				data_ = other.data_;
				add_ref();
			}
			return *this;
		}

		//! 소멸자 (자동 메모리 관리)
		~SmartMap()
		{
			release();
		}

		/**
		 * @brief 키-값 쌍 삽입 또는 업데이트
		 * @param key 키
		 * @param value 값
		 * @return 새로 삽입되었으면 true, 업데이트되었으면 false
		 */
		bool insert(const Key& key, const Value& value)
		{
			if (!data_) {
				data_ = new MapData();
			}

			ensure_unique();

			int index = data_->find_index(key);
			if (index >= 0) {
				// 기존 키 업데이트
				data_->pairs[index].value = value;
				return false;
			}

			// 새 키 삽입
			// 용량 부족 시 확장
			if (data_->size >= data_->capacity) {
				int new_capacity = data_->capacity * 2;
				if (new_capacity < 4) new_capacity = 4;
				data_->resize(new_capacity);
			}

			// 삽입 위치 찾기
			int insert_pos = data_->find_insert_position(key);

			// 삽입 위치 이후의 모든 항목을 뒤로 이동
			for (int i = data_->size; i > insert_pos; --i) {
				data_->pairs[i] = static_cast<KeyValuePair&&>(data_->pairs[i - 1]);
			}

			// 새 항목 삽입
			data_->pairs[insert_pos] = KeyValuePair(key, value);
			++data_->size;
			return true;
		}

		/**
		 * @brief 키-값 쌍 삽입 또는 업데이트 (move semantics)
		 * @param key 키
		 * @param value 값 (이동됨)
		 * @return 새로 삽입되었으면 true, 업데이트되었으면 false
		 */
		bool insert(const Key& key, Value&& value)
		{
			if (!data_) {
				data_ = new MapData();
			}

			ensure_unique();

			int index = data_->find_index(key);
			if (index >= 0) {
				// 기존 키 업데이트
				data_->pairs[index].value = static_cast<Value&&>(value);
				return false;
			}

			// 새 키 삽입
			if (data_->size >= data_->capacity) {
				int new_capacity = data_->capacity * 2;
				if (new_capacity < 4) new_capacity = 4;
				data_->resize(new_capacity);
			}

			int insert_pos = data_->find_insert_position(key);

			for (int i = data_->size; i > insert_pos; --i) {
				data_->pairs[i] = static_cast<KeyValuePair&&>(data_->pairs[i - 1]);
			}

			data_->pairs[insert_pos] = KeyValuePair(key, static_cast<Value&&>(value));
			++data_->size;
			return true;
		}

		/**
		 * @brief 키에 대한 값 반환 (참조)
		 * @param key 찾을 키
		 * @return 값 참조 (키가 없으면 기본값 반환)
		 */
		const Value& at(const Key& key) const
		{
			if (!data_) {
				static Value default_value{};
				return default_value;
			}

			int index = data_->find_index(key);
			if (index >= 0) {
				return data_->pairs[index].value;
			}

			static Value default_value{};
			return default_value;
		}

		/**
		 * @brief 키에 대한 값 반환 또는 삽입 (수정 가능)
		 * @param key 찾을 키
		 * @return 값 참조 (키가 없으면 기본값으로 새로 삽입)
		 */
		Value& operator[](const Key& key)
		{
			if (!data_) {
				data_ = new MapData();
			}

			ensure_unique();

			int index = data_->find_index(key);
			if (index >= 0) {
				return data_->pairs[index].value;
			}

			// 키가 없으면 기본값으로 삽입
			insert(key, Value{});
			index = data_->find_index(key);
			return data_->pairs[index].value;
		}

		/**
		 * @brief 키가 존재하는지 확인
		 * @param key 확인할 키
		 * @return 존재하면 true, 없으면 false
		 */
		bool contains(const Key& key) const
		{
			if (!data_) return false;
			return data_->find_index(key) >= 0;
		}

		/**
		 * @brief 특정 키 제거
		 * @param key 제거할 키
		 * @return 제거되었으면 true, 키가 없으면 false
		 */
		bool erase(const Key& key)
		{
			if (!data_) return false;

			ensure_unique();

			int index = data_->find_index(key);
			if (index < 0) return false;

			// 제거할 위치 이후의 모든 항목을 앞으로 이동
			for (int i = index; i < data_->size - 1; ++i) {
				data_->pairs[i] = static_cast<KeyValuePair&&>(data_->pairs[i + 1]);
			}

			--data_->size;
			return true;
		}

		/**
		 * @brief 현재 항목 개수 반환
		 * @return 항목 개수
		 */
		int size() const
		{
			return data_ ? data_->size : 0;
		}

		/**
		 * @brief 맵이 비어있는지 확인
		 * @return 비어있으면 true, 그렇지 않으면 false
		 */
		bool empty() const
		{
			return size() == 0;
		}

		/**
		 * @brief 모든 항목 제거
		 */
		void clear()
		{
			if (!data_) return;

			ensure_unique();
			data_->size = 0;
		}

		/**
		 * @brief 현재 참조 카운트 반환 (디버깅용)
		 * @return 참조 카운트 값 (유효하지 않으면 0)
		 */
		int ref_count() const
		{
			return data_ && data_->ref_count ? *data_->ref_count : 0;
		}

		/**
		 * @brief 동등성 비교 연산자
		 * @param other 비교할 SmartMap 인스턴스
		 * @return 모든 키-값 쌍이 같으면 true, 다르면 false
		 */
		bool operator==(const SmartMap& other) const
		{
			if (size() != other.size()) return false;

			for (int i = 0; i < size(); ++i) {
				const KeyValuePair& this_pair = data_->pairs[i];
				int other_index = other.data_->find_index(this_pair.key);

				if (other_index < 0 || !(this_pair.value == other.data_->pairs[other_index].value)) {
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief 부등성 비교 연산자
		 * @param other 비교할 SmartMap 인스턴스
		 * @return 키-값 쌍이 다르면 true, 같으면 false
		 */
		bool operator!=(const SmartMap& other) const
		{
			return !(*this == other);
		}

		// ========== STL 호환성을 위한 기본 iterator 지원 ==========

		/**
		 * @brief 간단한 반복자 클래스
		 */
		class iterator
		{
		private:
			KeyValuePair* ptr_;

		public:
			iterator(KeyValuePair* ptr) : ptr_(ptr) {}

			KeyValuePair& operator*() { return *ptr_; }
			KeyValuePair* operator->() { return ptr_; }
			iterator& operator++() { ++ptr_; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
			bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
			bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
		};

		/**
		 * @brief 읽기 전용 반복자 클래스
		 */
		class const_iterator
		{
		private:
			const KeyValuePair* ptr_;

		public:
			const_iterator(const KeyValuePair* ptr) : ptr_(ptr) {}

			const KeyValuePair& operator*() const { return *ptr_; }
			const KeyValuePair* operator->() const { return ptr_; }
			const_iterator& operator++() { ++ptr_; return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++ptr_; return tmp; }
			bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
			bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
		};

		/**
		 * @brief 시작 반복자 반환
		 * @return 시작 반복자
		 */
		iterator begin()
		{
			if (!data_ || data_->size == 0) return iterator(nullptr);
			ensure_unique();
			return iterator(data_->pairs);
		}

		/**
		 * @brief 시작 반복자 반환 (읽기 전용)
		 * @return 시작 반복자
		 */
		const_iterator begin() const
		{
			if (!data_ || data_->size == 0) return const_iterator(nullptr);
			return const_iterator(data_->pairs);
		}

		/**
		 * @brief 끝 반복자 반환
		 * @return 끝 반복자
		 */
		iterator end()
		{
			if (!data_ || data_->size == 0) return iterator(nullptr);
			ensure_unique();
			return iterator(data_->pairs + data_->size);
		}

		/**
		 * @brief 끝 반복자 반환 (읽기 전용)
		 * @return 끝 반복자
		 */
		const_iterator end() const
		{
			if (!data_ || data_->size == 0) return const_iterator(nullptr);
			return const_iterator(data_->pairs + data_->size);
		}

		// ========== 타입 별칭 (STL 호환성) ==========
		using key_type = Key;
		using mapped_type = Value;
		using value_type = KeyValuePair;
		using size_type = int;
		using reference = KeyValuePair&;
		using const_reference = const KeyValuePair&;
		using pointer = KeyValuePair*;
		using const_pointer = const KeyValuePair*;
	};
}
