#pragma once
#include <list>
#include <utility>
#include <vector>
#include <cstddef>
#include <cstring>
#include <openssl/bio.h>
#include <openssl/evp.h>

namespace synapse::network::cpp::zeromq::client::service::helpers
{
	class ConvertHelper 
	{
	public:
        static std::list<std::list<std::pair<int, int>>> BytesToSAM2Points(const uint8_t* buffer, int buffer_size)
        {
            std::list<std::list<std::pair<int, int>>> data;
            size_t offset = 0;

            while (offset < static_cast<size_t>(buffer_size)) {
                // 리스트 크기 읽기
                int inner_size;
                std::memcpy(&inner_size, buffer + offset, sizeof(inner_size));
                offset += sizeof(inner_size);

                std::list<std::pair<int, int>> inner_list;
                for (int i = 0; i < inner_size; ++i) {
                    int first, second;
                    // pair.first 읽기
                    std::memcpy(&first, buffer + offset, sizeof(first));
                    offset += sizeof(first);
                    // pair.second 읽기
                    std::memcpy(&second, buffer + offset, sizeof(second));
                    offset += sizeof(second);
                    inner_list.emplace_back(first, second);
                }
                data.push_back(inner_list);
            }
            return data;
        }

        static std::list<std::list<std::list<std::pair<int, int>>>> BytesToSAM2Points2(const uint8_t* buffer, int buffer_size)
        {
            std::list<std::list<std::list<std::pair<int, int>>>> data;
            size_t offset = 0;

            while (offset < static_cast<size_t>(buffer_size)) {
                // 리스트 크기 읽기
                int inner_size;
                std::memcpy(&inner_size, buffer + offset, sizeof(inner_size));
                offset += sizeof(inner_size);

                std::list<std::list<std::pair<int, int>>> inner_list;
                for (int i = 0; i < inner_size; ++i) {
                    // 리스트 크기 읽기
                    int inner_inner_size;
                    std::memcpy(&inner_inner_size, buffer + offset, sizeof(inner_inner_size));
                    offset += sizeof(inner_inner_size);

                    std::list<std::pair<int, int>> inner_inner_list;
                    for (int ii = 0; ii < inner_inner_size; ++ii) {
                        int first, second;
                        // pair.first 읽기
                        std::memcpy(&first, buffer + offset, sizeof(first));
                        offset += sizeof(first);
                        // pair.second 읽기
                        std::memcpy(&second, buffer + offset, sizeof(second));
                        offset += sizeof(second);
                        inner_inner_list.emplace_back(first, second);
                    }
                    inner_list.push_back(inner_inner_list);
                }
                data.push_back(inner_list);
            }
            return data;
        }

        // 할콘 
        //static void SAM2PointsToHRegion(const std::list<std::pair<int, int>>& points, HObject& region) 
        //{
        //    // 데이터를 일괄로 처리하기 위해 std::vector 사용
        //    std::vector<int> rows_vec, cols_vec;
        //    rows_vec.reserve(points.size()); // 미리 필요한 크기 예약
        //    cols_vec.reserve(points.size());
        //
        //    // point 데이터를 vector에 저장
        //    for (const auto& point : points)
        //    {
        //        rows_vec.push_back(point.second); // y 좌표
        //        cols_vec.push_back(point.first);  // x 좌표
        //    }
        //
        //    // vector 데이터를 HTuple로 변환
        //    HTuple rows = HTuple(&rows_vec[0], static_cast<int>(rows_vec.size()));
        //    HTuple cols = HTuple(&cols_vec[0], static_cast<int>(cols_vec.size()));
        //
        //    // HALCON 리전 생성
        //    try
        //    {
        //        GenRegionPolygonFilled(&region, rows, cols);
        //    }
        //    catch (HException& ex)
        //    {
        //        std::cerr << "HALCON Error: " << ex.ErrorMessage() << std::endl;
        //        throw;
        //    }
        //}

        // List<string> → byte[]
        static std::vector<uint8_t> EncodeStringsToByteArray(const std::vector<std::string>& list) {
            std::vector<uint8_t> byteArray;

            // 문자열 개수 추가 (4바이트)
            int count = list.size();
            byteArray.insert(byteArray.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));

            for (const auto& str : list) {
                // 문자열 길이 추가 (4바이트)
                int length = str.size();
                byteArray.insert(byteArray.end(), reinterpret_cast<uint8_t*>(&length), reinterpret_cast<uint8_t*>(&length) + sizeof(length));

                // 문자열 데이터 추가
                byteArray.insert(byteArray.end(), str.begin(), str.end());
            }

            return byteArray;
        }

        // byte[] → List<string>
        static std::vector<std::string> DecodeByteArrayToStrings(const std::vector<uint8_t>& byteArray) {
            std::vector<std::string> list;
            size_t offset = 0;

            // 문자열 개수 읽기
            int count;
            std::memcpy(&count, &byteArray[offset], sizeof(count));
            offset += sizeof(count);

            for (int i = 0; i < count; i++) {
                // 문자열 길이 읽기
                int length;
                std::memcpy(&length, &byteArray[offset], sizeof(length));
                offset += sizeof(length);

                // 문자열 데이터 읽기
                std::string str(byteArray.begin() + offset, byteArray.begin() + offset + length);
                offset += length;

                list.push_back(str);
            }

            return list;
        }

        // byte[] → List<string>
        static std::vector<std::string> DecodeByteArrayToStrings(const uint8_t* byteArray, size_t byteArraySize) {
            std::vector<std::string> list;
            size_t offset = 0;

            // 문자열 개수 읽기
            if (offset + sizeof(int) > byteArraySize) {
                throw std::runtime_error("Invalid byte array: insufficient data for string count");
            }
            int count;
            std::memcpy(&count, byteArray + offset, sizeof(count));
            offset += sizeof(count);

            for (int i = 0; i < count; i++) {
                // 문자열 길이 읽기
                if (offset + sizeof(int) > byteArraySize) {
                    throw std::runtime_error("Invalid byte array: insufficient data for string length");
                }
                int length;
                std::memcpy(&length, byteArray + offset, sizeof(length));
                offset += sizeof(length);

                // 문자열 데이터 읽기
                if (offset + length > byteArraySize) {
                    throw std::runtime_error("Invalid byte array: insufficient data for string content");
                }
                std::string str(reinterpret_cast<const char*>(byteArray + offset), length);
                offset += length;

                list.push_back(str);
            }

            return list;
        }

        static char** DecodeByteArrayToCharPtr(uint8_t* byteArray, size_t byteArraySize) {
            if (byteArray == nullptr || byteArraySize < sizeof(int)) {
                throw std::invalid_argument("Invalid byte array or size.");
            }

            size_t offset = 0;

            // 문자열 개수 읽기
            int stringCount = 0;
            std::memcpy(&stringCount, byteArray + offset, sizeof(int));
            offset += sizeof(int);

            if (offset > byteArraySize) {
                throw std::invalid_argument("Invalid byte array format.");
            }

            // char** 배열 생성
            char** charArray = new char* [stringCount];

            for (int i = 0; i < stringCount; ++i) {
                // 문자열 길이 읽기
                int stringLength = 0;
                std::memcpy(&stringLength, byteArray + offset, sizeof(int));
                offset += sizeof(int);

                if (offset + stringLength > byteArraySize) {
                    throw std::invalid_argument("Invalid byte array format.");
                }

                // 문자열 데이터 읽기
                charArray[i] = new char[stringLength + 1]; // NULL 문자 포함
                std::memcpy(charArray[i], byteArray + offset, stringLength);
                charArray[i][stringLength] = '\0'; // NULL 문자 추가
                offset += stringLength;
            }

            return charArray;
        }
	};
}