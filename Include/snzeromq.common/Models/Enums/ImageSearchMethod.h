#pragma once
namespace synapse::network::cpp::zeromq::common::enums {
    enum class ImageSearchMethod
    {
        None,
        Id, // id 로 검색
        Name, // 이름으로 검색
        Time, // 시간으로 검색
        Tag, // Tag 정보로 검색
    };
}