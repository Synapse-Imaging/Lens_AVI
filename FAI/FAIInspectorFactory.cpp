#include "StdAfx.h"
#include "IFAIInspector.h"
#include "FAIDataManager.h"
#include "Define.h"

// 모델 추가 시 해당 모델의 FAIInspector 클래스 헤더 추가
#include "BOIFAIInspector.h"
#include "ACTFAIInspector.h"
#include "AKCFAIInspector.h"
#include "ATWFAIInspector.h"
#include "BOSFAIInspector.h"
#include "RENOFAIInspector.h"
#include "KRIOSFAIInspector.h"


// FAI Inspector 클래스를 변경하는 팩토리 함수
void ReplaceFAIInspector(IFAIInspector*& pCurrentInspector, CString strModelType, Algorithm* pAlgorithm)
{
    delete pCurrentInspector;
    pCurrentInspector = nullptr;

    if (strModelType == _T("BOI"))             pCurrentInspector = new BOIFAIInspector(pAlgorithm);
    else if (strModelType == _T("ACT"))        pCurrentInspector = new ACTFAIInspector(pAlgorithm);
    else if (strModelType == _T("AKC"))        pCurrentInspector = new AKCFAIInspector(pAlgorithm);
    else if (strModelType == _T("ATW-D"))        pCurrentInspector = new ATWFAIInspector(pAlgorithm);
    else if (strModelType == _T("BOS"))        pCurrentInspector = new BOSFAIInspector(pAlgorithm);
    else if (strModelType == _T("RENO"))        pCurrentInspector = new RENOFAIInspector(pAlgorithm);
    else if (strModelType == _T("KRIOS"))        pCurrentInspector = new KRIOSFAIInspector(pAlgorithm);
    else
    {
		TRACE("Unknown model type: %s. FAI Inspector not created.\n", strModelType);
    }
}
