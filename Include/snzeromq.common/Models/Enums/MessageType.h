#pragma once
namespace synapse::network::cpp::zeromq::common::enums {
	enum class MessageType
	{
        // 요청
        //=====================================================================================

        /// <summary>
        /// Client -> Server, 접속 요청
        /// 클라이언트가 서버에 접속을 요청할 때 사용됩니다.
        /// </summary>
        RequestConnect = 1,

        /// <summary>
        /// Client -> Server, Loop Test
        /// 클라이언트가 서버와의 연결 상태를 테스트하기 위해 Loopback 요청을 보낼 때 사용됩니다.
        /// </summary>
        RequestLoopback = 2,

        /// <summary>
        /// Client -> Server, 메타데이터 요청
        /// 클라이언트가 서버에서 메타데이터를 요청할 때 사용됩니다.
        /// </summary>
        RequestMatadata = 3,

        /// <summary>
        /// Client -> Server, 이미지 요청
        /// 클라이언트가 서버에서 이미지를 요청할 때 사용됩니다.
        /// </summary>
        RequestImage = 4,

        /// <summary>
        /// Client -> Server, AI SAM2 서비스 요청
        /// 클라이언트가 서버에서 SAM2 서비스를 요청할 때 사용됩니다.
        /// </summary>
        RequestSAM2 = 5,

        /// <summary>
        /// Client -> Server, 토큰 정보 업데이트 요청
        /// 클라이언트가 서버에 토큰 정보 업데이트를 요청합니다. 
        /// </summary>
        RequestTokenUpdate = 6,

        // 전송
        //=====================================================================================

        /// <summary>
        /// Client -> Server, 이미지 전송
        /// 클라이언트가 서버로 이미지를 전송할 때 사용됩니다.
        /// </summary>
        SendImage = 100,

        // 응답
        //=====================================================================================

        /// <summary>
        /// Server -> Client, 표준 응답 메시지
        /// 서버가 요청에 대한 표준 응답을 보낼 때 사용됩니다.
        /// </summary>
        ResponseMessage = 200,

        /// <summary>
        /// Server -> Client, 접속 용청
        /// 서버가 요청에 대한 접속 응답을 보낼 때 사용됩니다.
        /// </summary>
        ResponseConnect = 201,

        /// <summary>
        /// Server -> Client, 메타데이터 응답
        /// 서버가 메타데이터 요청에 대한 응답으로 메타데이터를 보낼 때 사용됩니다.
        /// </summary>
        Responseatadata = 203,

        /// <summary>
        /// Server -> Client, 이미지 응답
        /// 서버가 이미지 요청에 대한 응답으로 이미지를 보낼 때 사용됩니다.
        /// </summary>
        ResponseImage = 204,

        /// <summary>
        /// Client -> Server, AI SAM2 서비스 응답
        /// 서버가 SAM2 시비스 결과를 응답 합니다. 
        /// </summary>
        ResponseSAM2 = 205,

        /// <summary>
        /// Client -> Server, 토큰 정보 업데이트 응답
        /// 서버가 토큰 정보 업데이트 결과를 응답 합니다. 
        /// </summary>
        ResponseTokenUpdate = 206,

        // 서버 에러 응답
        //=====================================================================================

        /// <summary>
        /// Server -> Client, 표준 에러 응답
        /// 서버가 요청 처리 중 발생한 에러에 대해 표준 에러 응답할 때 사용됩니다.
        /// </summary>
        ResponseError = 900,

        /// <summary>
        /// Server -> Client, 토큰 에러 응답
        /// 서버가 클라이언트의 인증 토큰이 유효하지 않을 경우 응답할 때 사용됩니다.
        /// </summary>
        ResponseErrorInvalidToken = 901,

        /// <summary>
        /// Server -> Client, 토큰 에러 응답
        /// 서버에서 데이터 역 직렬화 실패.
        /// </summary>
        ResponseErrorDeserialize = 902,

        /// <summary>
        /// Server -> Client, 토큰 에러 응답
        /// 메시지 타입 에러
        /// </summary>
        ResponseErrorMessageType = 903,

        /// <summary>
        /// Server -> Client, 토큰 에러 응답
        /// 메시지 핸들러 생성 에러
        /// </summary>
        ResponseErrorMessageHandler = 904,

        // 무명
        //=====================================================================================

        /// <summary>
        /// 알 수 없는 메시지 타입
        /// 처리되지 않거나 정의되지 않은 메시지 타입을 나타냅니다.
        /// </summary>
        Unknown = 999,
	};
}