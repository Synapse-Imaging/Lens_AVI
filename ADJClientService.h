#ifdef USE_SUAKIT

#pragma once
#include "TCPClient.h"

#define RECV_INSPECT_RESULT 1
#define RECV_WATCHDOG 2
#define RECV_INSPECTFAI_RESULT 3
#define RECV_INSPECT_SEGMENT_RESULT 4

// CADJClientService

//TCP Packet Structure
#pragma pack( push,1 )
struct stTCPPacket
{
	BYTE  byteCommand[ PACKET_STRING_SIZE ];
	BYTE  byteOperator[ PACKET_STRING_SIZE ];
	BYTE  byteLotID[ PACKET_STRING_SIZE ];
	BYTE  byteTrayNo[ PACKET_STRING_SIZE ];
	BYTE  byteModuleNo[ PACKET_STRING_SIZE ];
	BYTE  byteDefectCnt[ PACKET_STRING_SIZE ];
	BYTE  byteDeepLearningModelID[ PACKET_STRING_SIZE ];

	BYTE  byteMagazineNo[PACKET_STRING_SIZE]; //20201016 add

	BYTE  byteIP[ PACKET_STRING_SIZE ];
	BYTE  bytePort[ PACKET_STRING_SIZE ];
	BYTE  byteFileName[ PACKET_FILE_NAME_SIZE ];
	BYTE  byteDeepLearningResult[ PACKET_STRING_SIZE ];
	BYTE  byteModelVersion[ PACKET_STRING_SIZE ];	//230331 모델버전 ADJ와 공유

	//BYTE  byteImageCount[ PACKET_STRING_SIZE ];
	BYTE  byteImageWidth[ PACKET_STRING_SIZE ];
	BYTE  byteImageHeight[ PACKET_STRING_SIZE ];
	//BYTE  byteMaxClassProb[PACKET_STRING_SIZE]; //20200809 add //20200912 commented
	BYTE* pByteImageData;
	int   nDataSize;
	int   nDataSizePosition;

	//구조체 초기화.
	stTCPPacket()
	{
		memset( byteCommand, 0, PACKET_STRING_SIZE );
		memset( byteOperator, 0, PACKET_STRING_SIZE );
		memset( byteLotID, 0, PACKET_STRING_SIZE );
		memset( byteTrayNo, 0, PACKET_STRING_SIZE );
		memset( byteModuleNo, 0, PACKET_STRING_SIZE );
		memset( byteDefectCnt, 0, PACKET_STRING_SIZE );
		memset( byteDeepLearningModelID, 0, PACKET_STRING_SIZE );

		memset(byteMagazineNo, 0, PACKET_STRING_SIZE); //20201016 add

		memset( byteIP, 0, PACKET_STRING_SIZE );
		memset( bytePort, 0, PACKET_STRING_SIZE );
		//for (int i = 0; i < PACKET_IMAGE_COUNT; i++)
		memset(byteFileName, 0, PACKET_FILE_NAME_SIZE);
		//memset( byteImageCount, 0, PACKET_STRING_SIZE );
		//memset(byteMaxClassProb, 0, PACKET_STRING_SIZE); //20200809 add //20200912 commented
		memset(byteImageWidth, 0, PACKET_STRING_SIZE);
		memset(byteImageHeight, 0, PACKET_STRING_SIZE);
		memset(byteModelVersion, 0, PACKET_STRING_SIZE);
		//memset(pByteImageData, 0, PACKET_IMAGE_COUNT);
		pByteImageData = nullptr;
		nDataSize = 0;
		nDataSizePosition = 0;
	}

	//실제 패킷으로 합치는 메쏘드
	int MakeImagePacket( BYTE* pbyteEmpty)
	{
		int curIdx = 0;
		pbyteEmpty[ curIdx ] = '@';
		CString strDataSize = "";

		curIdx++;
		memset(byteCommand, 0, PACKET_STRING_SIZE);
		memset(byteOperator, 0, PACKET_STRING_SIZE);
		//memcpy( this->byteCommand, (const char*)"INSPECT", PACKET_STRING_SIZE);
		
		byteCommand[0] = 'I';
		byteCommand[1] = 'N';
		byteCommand[2] = 'S';
		byteCommand[3] = 'P';
		byteCommand[4] = 'E';
		byteCommand[5] = 'C';
		byteCommand[6] = 'T';
		byteCommand[7] = NULL;

		byteOperator[0] = 'R';
		byteOperator[1] = 'E';
		byteOperator[2] = 'Q';
		byteOperator[3] = 'U';
		byteOperator[4] = 'E';
		byteOperator[5] = 'S';
		byteOperator[6] = 'T';
		byteOperator[7] = NULL;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+curIdx] = byteCommand[i];
		}

		//memcpy( &pbyteEmpty[ curIdx ], this->byteCommand, PACKET_STRING_SIZE );

		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteOperator[i];
		}
		//memcpy( &pbyteEmpty[ curIdx ], this->byteOperator, PACKET_STRING_SIZE );
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//Packet DataSize Empty Space Create
		nDataSizePosition = curIdx;
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteLotID, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteLotID[i];
		}

		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteTrayNo, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteTrayNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteModuleNo, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteModuleNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteDefectCnt, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteDefectCnt[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteDeepLearningModelID, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteDeepLearningModelID[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy(&pbyteEmpty[curIdx], this->byteMagazineNo, PACKET_STRING_SIZE);
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteMagazineNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteIP, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = byteIP[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->bytePort, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i+ curIdx] = bytePort[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[ curIdx ] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->bytePort, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteModelVersion[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		/*for (int k = 0; k < PACKET_IMAGE_COUNT; k++)
		{
			//memcpy( &pbyteEmpty[ curIdx ], this->byteFileName, PACKET_STRING_SIZE );
			for (int i = 0; i < PACKET_STRING_SIZE; i++)
			{
				pbyteEmpty[i + curIdx] = byteFileName[k][i];
			}
			curIdx += PACKET_STRING_SIZE;

			pbyteEmpty[curIdx] = ',';
			curIdx++;
		}*/

		//memcpy( &pbyteEmpty[ curIdx ], this->byteFileName, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_FILE_NAME_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteFileName[i];
		}
		curIdx += PACKET_FILE_NAME_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		/*//memcpy( &pbyteEmpty[ curIdx ], this->byteImageCount, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageCount[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;*/

		//memcpy( &pbyteEmpty[ curIdx ], this->byteImageWidth, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageWidth[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteImageHeight, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageHeight[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		strDataSize.Format( "%d", curIdx + (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3));
		//memcpy( &pbyteEmpty[ curIdx ], strDataSize , PACKET_STRING_SIZE );
		for (int i = 0; i < strDataSize.GetLength(); i++)
		{
			pbyteEmpty[i + nDataSizePosition] = strDataSize.GetAt(i);
			if (i + 1 == strDataSize.GetLength())
				pbyteEmpty[i + nDataSizePosition + 1 ] = NULL;
		}

		for (int j = 0; j < (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3); j++)
		{
			pbyteEmpty[curIdx + j] = pByteImageData[j];
		}
		curIdx += (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3);
		
		//memcpy( &pbyteEmpty[ curIdx ], this->pByteImageData, PACKET_IMAGE_SIZE );
		

		//nDataSize = curIdx; //20200919 commented
		return curIdx; // 20200919 수정 (nDataSize가 다른 쓰레드에서 변환될 수 있으니 단순하게 처리하게 수정)
	}
	
	int MakeFAIImagePacket(BYTE* pbyteEmpty)
	{
		int curIdx = 0;
		pbyteEmpty[curIdx] = '@';
		CString strDataSize = "";

		curIdx++;
		memset(byteCommand, 0, PACKET_STRING_SIZE);
		memset(byteOperator, 0, PACKET_STRING_SIZE);
		//memcpy( this->byteCommand, (const char*)"INSPECT", PACKET_STRING_SIZE);

		byteCommand[0] = 'I';
		byteCommand[1] = 'N';
		byteCommand[2] = 'S';
		byteCommand[3] = 'P';
		byteCommand[4] = 'E';
		byteCommand[5] = 'C';
		byteCommand[6] = 'T';
		byteCommand[7] = 'F';
		byteCommand[8] = 'A';
		byteCommand[9] = 'I';
		byteCommand[10] = NULL;

		byteOperator[0] = 'R';
		byteOperator[1] = 'E';
		byteOperator[2] = 'Q';
		byteOperator[3] = 'U';
		byteOperator[4] = 'E';
		byteOperator[5] = 'S';
		byteOperator[6] = 'T';
		byteOperator[7] = NULL;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteCommand[i];
		}

		//memcpy( &pbyteEmpty[ curIdx ], this->byteCommand, PACKET_STRING_SIZE );

		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteOperator[i];
		}
		//memcpy( &pbyteEmpty[ curIdx ], this->byteOperator, PACKET_STRING_SIZE );
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//Packet DataSize Empty Space Create
		nDataSizePosition = curIdx;
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteLotID, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteLotID[i];
		}

		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteTrayNo, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteTrayNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteModuleNo, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteModuleNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteDefectCnt, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteDefectCnt[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteDeepLearningModelID, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteDeepLearningModelID[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy(&pbyteEmpty[curIdx], this->byteMagazineNo, PACKET_STRING_SIZE);
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteMagazineNo[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteIP, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteIP[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->bytePort, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = bytePort[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		/*for (int k = 0; k < PACKET_IMAGE_COUNT; k++)
		{
			//memcpy( &pbyteEmpty[ curIdx ], this->byteFileName, PACKET_STRING_SIZE );
			for (int i = 0; i < PACKET_STRING_SIZE; i++)
			{
				pbyteEmpty[i + curIdx] = byteFileName[k][i];
			}
			curIdx += PACKET_STRING_SIZE;

			pbyteEmpty[curIdx] = ',';
			curIdx++;
		}*/

		//memcpy( &pbyteEmpty[ curIdx ], this->byteFileName, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_FILE_NAME_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteFileName[i];
		}
		curIdx += PACKET_FILE_NAME_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		/*//memcpy( &pbyteEmpty[ curIdx ], this->byteImageCount, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageCount[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;*/

		//memcpy( &pbyteEmpty[ curIdx ], this->byteImageWidth, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageWidth[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->byteImageHeight, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteImageHeight[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		strDataSize.Format("%d", curIdx + (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3));
		//memcpy( &pbyteEmpty[ curIdx ], strDataSize , PACKET_STRING_SIZE );
		for (int i = 0; i < strDataSize.GetLength(); i++)
		{
			pbyteEmpty[i + nDataSizePosition] = strDataSize.GetAt(i);
			if (i + 1 == strDataSize.GetLength())
				pbyteEmpty[i + nDataSizePosition + 1] = NULL;
		}

		for (int j = 0; j < (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3); j++)
		{
			pbyteEmpty[curIdx + j] = pByteImageData[j];
		}
		curIdx += (atoi((const char*)byteImageWidth) * atoi((const char*)byteImageHeight) * 3);

		//memcpy( &pbyteEmpty[ curIdx ], this->pByteImageData, PACKET_IMAGE_SIZE );


		//nDataSize = curIdx; //20200919 commented
		return curIdx; // 20200919 수정 (nDataSize가 다른 쓰레드에서 변환될 수 있으니 단순하게 처리하게 수정)
	}

	int MakeWatchDogPacket(BYTE* pbyteEmpty) 
	{
		int curIdx = 0;
		pbyteEmpty[curIdx] = '@';
		CString strDataSize = "";

		curIdx++;
		memset(byteCommand, 0, PACKET_STRING_SIZE);
		memset(byteOperator, 0, PACKET_STRING_SIZE);
		//memcpy( this->byteCommand, (const char*)"INSPECT", PACKET_STRING_SIZE);

		byteCommand[0] = 'W';
		byteCommand[1] = 'A';
		byteCommand[2] = 'T';
		byteCommand[3] = 'C';
		byteCommand[4] = 'H';
		byteCommand[5] = 'D';
		byteCommand[6] = 'O';
		byteCommand[7] = 'G';
		byteCommand[8] = NULL;

		byteOperator[0] = 'R';
		byteOperator[1] = 'E';
		byteOperator[2] = 'Q';
		byteOperator[3] = 'U';
		byteOperator[4] = 'E';
		byteOperator[5] = 'S';
		byteOperator[6] = 'T';
		byteOperator[7] = NULL;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteCommand[i];
		}

		//memcpy( &pbyteEmpty[ curIdx ], this->byteCommand, PACKET_STRING_SIZE );

		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = byteOperator[i];
		}
		//memcpy( &pbyteEmpty[ curIdx ], this->byteOperator, PACKET_STRING_SIZE );
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//Packet DataSize Empty Space Create
		nDataSizePosition = curIdx;
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		//memcpy( &pbyteEmpty[ curIdx ], this->bytePort, PACKET_STRING_SIZE );
		for (int i = 0; i < PACKET_STRING_SIZE; i++)
		{
			pbyteEmpty[i + curIdx] = bytePort[i];
		}
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		strDataSize.Format("%d", curIdx);
		//memcpy( &pbyteEmpty[ curIdx ], strDataSize , PACKET_STRING_SIZE );
		for (int i = 0; i < strDataSize.GetLength(); i++)
		{
			pbyteEmpty[i + nDataSizePosition] = strDataSize.GetAt(i);
			if (i + 1 == strDataSize.GetLength())
				pbyteEmpty[i + nDataSizePosition + 1] = NULL;
		}

		return curIdx; // 20200919 수정 (nDataSize가 다른 쓰레드에서 변환될 수 있으니 단순하게 처리하게 수정)
	}

	int GetPacket( const BYTE* pByteMessage )
	{
		int curIdx = 1; //'@' -> start
		CString strCommand, strOperator;
		memcpy( this->byteCommand, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
		curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

		memcpy( this->byteOperator, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
		curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

		strCommand  = (const char*)this->byteCommand;
		strOperator = (const char*)this->byteOperator;

		if( !strCommand.Compare( "INSPECT" ) && !strOperator.Compare( "REPLY" ) )
		{
			memcpy( this->byteLotID, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteTrayNo, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteModuleNo, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteDefectCnt, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteDeepLearningModelID, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteMagazineNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteDeepLearningResult, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy( this->byteIP, &pByteMessage[ curIdx ], PACKET_STRING_SIZE );
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론
			
			//memcpy(this->byteMaxClassProb, &pByteMessage[curIdx], PACKET_STRING_SIZE); //20200809 add //20200912 commented
			//curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론               //20200809 add //20200912 commented

			if ( pByteMessage != nullptr )
			{
				delete pByteMessage;
			}
			return RECV_INSPECT_RESULT;
		}
		else if (!strCommand.Compare("INSPECTSEGMENT") && !strOperator.Compare("REPLY"))
		{
			memcpy(this->byteLotID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteTrayNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteModuleNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDefectCnt, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDeepLearningModelID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteMagazineNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDeepLearningResult, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteIP, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론 // DataSize Position

			memcpy(this->byteImageWidth, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteImageHeight, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			int imageWidth = atoi((const char*)this->byteImageWidth);
			int imageHeight = atoi((const char*)this->byteImageHeight);
			int imageDataSize = imageWidth * imageHeight * 3;

			this->pByteImageData = new BYTE[imageDataSize];
			memcpy(this->pByteImageData, &pByteMessage[curIdx], imageDataSize);
			curIdx += imageDataSize; //1을 더해준 것은 세미콜론

			if (pByteMessage != nullptr)
			{
				delete pByteMessage;
			}
			return RECV_INSPECT_SEGMENT_RESULT;
		}
		else if (!strCommand.Compare("INSPECTFAI") && !strOperator.Compare("REPLY"))
		{
			memcpy(this->byteLotID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteTrayNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteModuleNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDefectCnt, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDeepLearningModelID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteMagazineNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteDeepLearningResult, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			memcpy(this->byteIP, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론

			//memcpy(this->byteMaxClassProb, &pByteMessage[curIdx], PACKET_STRING_SIZE); //20200809 add //20200912 commented
			//curIdx += PACKET_STRING_SIZE + 1; //1을 더해준 것은 세미콜론               //20200809 add //20200912 commented

			if (pByteMessage != nullptr)
			{
				delete pByteMessage;
			}
			return RECV_INSPECTFAI_RESULT;
		}
		else
		{
			if ( pByteMessage != nullptr )
			{
				delete pByteMessage;
			}
			return RECV_WATCHDOG;
		}
	}
	int GetDataSize ( void ) { return nDataSize; }
};
#pragma pack( pop )

typedef struct _stADJNetworkData
{
	stTCPPacket tcpPacket;
	int iClientNO, iPcVisionNo, nThreadNO, iMzNo, iTrayNo, iModuleNo, iDefectCnt, iDeepLearningModelID, iVisionCamType, iCurDeepLearningModel, testADJStartTime, *iReviewSaveCnt;
	CString sLotID, sBarcodeID, ConcatFilename, strDefectCode, sADJInspectionTypeCode, sADJDefectDetail, ConcatReviewImage;

	std::vector<unsigned char> vecImageData;
	HObject HADJSaveRegion, *HADJFilteredDefectRgn, ConcatimageObj;
	SYSTEMTIME time;
	DWORD iModuleTotalStartTime;

	// --- 기본 생성자 ---
	_stADJNetworkData()
		: // --- 멤버 초기화 리스트 시작 ---
		tcpPacket(),                      // stTCPPacket의 기본 생성자 호출
		iClientNO(0),                     // int 멤버들을 0으로 초기화
		iPcVisionNo(0),
		nThreadNO(0),
		iMzNo(0),
		iTrayNo(0),
		iModuleNo(0),
		iDefectCnt(0),
		iDeepLearningModelID(0),
		iVisionCamType(0),
		iCurDeepLearningModel(0),
		testADJStartTime(0),
		iReviewSaveCnt(nullptr),            // int 포인터는 nullptr로 초기화
		sLotID(),                         // CString 멤버들은 기본 생성자 호출 (빈 문자열 "")
		sBarcodeID(),
		ConcatFilename(),
		strDefectCode(),
		sADJInspectionTypeCode(),
		sADJDefectDetail(),
		ConcatReviewImage(),
		vecImageData(),                   // vector는 기본 생성자 호출 (빈 벡터)
		HADJSaveRegion(),                 // HObject는 기본 생성자 호출 (비초기화 상태)
		HADJFilteredDefectRgn(nullptr),     // *** HObject 포인터는 nullptr로 초기화 ***
		ConcatimageObj(),                 // HObject는 기본 생성자 호출 (비초기화 상태)
		time{},                           // SYSTEMTIME 구조체를 0으로 초기화 (C++11 이상)
		iModuleTotalStartTime(0)            // DWORD를 0으로 초기화
		// --- 멤버 초기화 리스트 끝 ---
	{
		// 생성자 본문 (필요한 경우 추가 로직 구현)

		// C++11 이전 버전에서 SYSTEMTIME 초기화:
		// memset(&time, 0, sizeof(time));
	}
}stADJNetworkData;

typedef struct _stFAIADJNetworkData
{
	stTCPPacket tcpPacket;
	int iClientNO, iFAI_Number, iPcVisionNo, nThreadNO, iMzNo, iTrayNo, iModuleNo, iDefectCnt, iFaiParamIndex, iDeepLearningModelID, testADJStartTime, *iReviewSaveCnt;
	CString sLotID, sBarcodeID, ConcatFilename, ConcatReviewImage;

	HObject HADJSaveRegion, *HADJFilteredDefectRgn, ConcatimageObj;
	SYSTEMTIME time;
	DWORD iModuleTotalStartTime;

}stFAIADJNetworkData;

class CADJClientService : public CWnd
{
	DECLARE_DYNAMIC(CADJClientService)

public:
	CADJClientService();
	virtual ~CADJClientService();

	void Initialize( void );
	int  SendImagetoADJ( stTCPPacket stPacket, int nClientNO, int nThreadNO, int nWaitMilsec );
	cv::Mat SendSegmentImagetoADJ(stTCPPacket stPacket, int nClientNO, int nThreadNO, int nWaitMilsec);
	cv::Mat SendSegmentImagetoADJ(int nThreadNO, int nMzNO, int nTrayNO, int nModuleNO, CString sLotID, int nDefectCount, int nDeepLearningModelID, CString sFileName, int nWaitMilsec, cv::Mat matADJImage);
	int  SendFAIImagetoADJ(stTCPPacket stPacket, int nClientNO, int nWaitMilsec);
	void ReStart( void );
	BOOL m_bConnect[MAX_CLIENT_NUM][MAX_MULTI_CONNECTION_NUM];

	CTCPClient m_arrClient[ MAX_CLIENT_NUM ][MAX_MULTI_CONNECTION_NUM];
	int m_nTimeOutCnt;
	int m_nClientNO;
	CString m_strClientIP;
	BOOL m_bTCPThreadRun[ MAX_CLIENT_NUM ][MAX_MULTI_CONNECTION_NUM];
	void ThreeHandShakingRequest( int nClientNO ); 
	void ADJResetRequest( int nClientNO );
	BOOL m_bRealConnect[ MAX_CLIENT_NUM ][MAX_MULTI_CONNECTION_NUM];

	cv::Mat matMaskImageData[MAX_TRAY_LOT][MAX_MODULE_NUM][MAX_DEFECT_CNT];
	//method for deeplearning inspection
	CString GetDeepLearningModelNameFromIndex(int nDeepLearningModelID, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail);
	CString GetDeepLearningModelNameFromIndex(int nDeepLearningModelID, int iVisionCamType);
	int AssignDeepLearningModel(int iClientNO, int iVisionCamType,CString sADJInspectionTypeCode,CString sADJDefectDetail);
	int AssignDeepLearningSegmentModel(int iClientNO, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail);
	int AssignDeepLearningFAIModel(int FAI_number);
	//void ResetADJResult(int iDeepLearningModelNO,int iModuleNo, int nDefectCnt);
	int m_nADJDefectCnt[ VISION_NUMBER_MAX ];
	int DoDeeplearningInspection( int nThreadNO,
		int iInspectionBufferIdx,
		int iVisionCamType,
		HObject HVisionAllDefectRgn,	
		int iPcVisionNo,
		int iNoInspectImage,
		int iMzNo,
		int iTrayNo,
		int iModuleNo,
		CString sLotID,
		CString sBarcodeID,
		DWORD iModuleTotalStartTime,
		HObject (&HADJFilteredDefectRgn)[MAX_IMAGE_TAB],
		int &iInspectionTypeResult);

	int DoDeeplearningRosReviewFile_FAI( int nThreadNO,
		int iMzNo, 
		int iTrayNo, 
		int iModuleNo, 
		int iPcVisionNo, 
		int iVisionCamType, 
		int iNoInspectionImage,
		HObject *pHDisplayImage, 
		HObject(*ppHReviewRgn_FAI_Item)[12], 
		POINT(*ppiViewportCenter_FAI_Item)[12],
		CString sLotID,
		CString sBarcodeID, 
		int *piNoSaveReviewImage,
		DWORD iModuleTotalStartTime
	);

	//tcp alive check(2-hand-shake)
	void ADJConnectAliveCheck( void );
	static UINT	ADJ_Network_Thread(LPVOID lp);
	static UINT	FAI_ADJ_Network_Thread(LPVOID lp);
	CSafeQueue< stADJNetworkData* > m_qADJBuffer[MAX_MODULE_NUM];
	CSafeQueue< stFAIADJNetworkData* > m_qFAIADJBuffer[MAX_MODULE_NUM];
	int m_DeepLearningProcessing(int iClientNo,
		int iTrayNo,
		int iModuleNo,
		CString sLotID,
		CString sBarcodeID,
		CString sFileName,
		CString modelname,
		std::vector<unsigned char> DLImg);	// PyADJ 

	CRITICAL_SECTION m_csADJChangeStatus;
	CRITICAL_SECTION m_csFAIADJChangeStatus;
	int m_iModuleNo_ADJCnt;

	void SetAdjStatus(int nModelID, int nMazineNo, int nTrayNo, int nModuleNo, int nDefectCnt, int nStatus)
	{
		EnterCriticalSection(&m_csADJChangeStatus);
		m_nADJStatus[nModelID][nMazineNo][nTrayNo][nModuleNo][nDefectCnt] = nStatus;
		LeaveCriticalSection(&m_csADJChangeStatus);
	};
	int  GetAdjStatus(int nModelID, int nMazineNo, int nTrayNo, int nModuleNo, int nDefectCnt) {
		EnterCriticalSection(&m_csADJChangeStatus);
		int nStatus =  m_nADJStatus[nModelID][nMazineNo][nTrayNo][nModuleNo][nDefectCnt]; 
		LeaveCriticalSection(&m_csADJChangeStatus);
		return nStatus;
	};
	void ResetAdjStatus()
	{
		EnterCriticalSection(&m_csADJChangeStatus);
		
		for ( int a = 0; a < MAX_MODEL_NUM; a++ )
			for ( int b = 0; b < MAX_MAGAZINE_NO; b++ )
				for ( int c = 0; c < MAX_TRAY_LOT; c++ )
					for ( int d = 0; d < MAX_MODULE_NUM; d++ )
						for ( int e = 0; e < MAX_DEFECT_CNT; e++ )
							m_nADJStatus[a][b][c][d][e] = RCV_WAIT;

		LeaveCriticalSection(&m_csADJChangeStatus);
	};
	void SetFAIAdjStatus(int nModelID, int nMazineNo, int nTrayNo, int nModuleNo, int nDefectCnt, int nStatus)
	{
		EnterCriticalSection(&m_csFAIADJChangeStatus);
		m_nFAIADJStatus[nModelID][nMazineNo][nTrayNo][nModuleNo][nDefectCnt] = nStatus;
		LeaveCriticalSection(&m_csFAIADJChangeStatus);
	};
	int  GetFAIAdjStatus(int nModelID, int nMazineNo, int nTrayNo, int nModuleNo, int nDefectCnt) {
		EnterCriticalSection(&m_csFAIADJChangeStatus);
		int nStatus = m_nFAIADJStatus[nModelID][nMazineNo][nTrayNo][nModuleNo][nDefectCnt];
		LeaveCriticalSection(&m_csFAIADJChangeStatus);
		return nStatus;
	};
	void ResetFAIAdjStatus()
	{
		EnterCriticalSection(&m_csFAIADJChangeStatus);

		for (int a = 0; a < MAX_MODEL_NUM; a++)
			for (int b = 0; b < MAX_MAGAZINE_NO; b++)
				for (int c = 0; c < MAX_TRAY_LOT; c++)
					for (int d = 0; d < MAX_MODULE_NUM; d++)
						for (int e = 0; e < MAX_DEFECT_CNT; e++)
							m_nFAIADJStatus[a][b][c][d][e] = RCV_WAIT;

		LeaveCriticalSection(&m_csFAIADJChangeStatus);
	};

	CString strLog;

protected:
	int m_nADJStatus[MAX_MODEL_NUM][MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_NUM][MAX_DEFECT_CNT]; //20201016 add
	int m_nFAIADJStatus[MAX_MODEL_NUM][MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_NUM][MAX_DEFECT_CNT]; //20201016 add


	DECLARE_MESSAGE_MAP()
};

#endif