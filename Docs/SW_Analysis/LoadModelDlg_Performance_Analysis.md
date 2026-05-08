# LoadModelDlg — OnSelchangeListModelnames 성능 분석

> **대상 함수**: `LoadModelDlg.cpp` — `CLoadModelDlg::OnSelchangeListModelnames()`  
> **분석 방법**: 정적 코드 분석

---

## 1. 함수 호출 흐름 요약

```
[사용자가 리스트에서 모델 클릭]
    ↓ LBN_SELCHANGE → OnSelchangeListModelnames() (UI 스레드)
        ↓ GetCurSel() / GetText()                   — 빠름 (메모리)
        ↓ UpdateData(FALSE)                         — 빠름 (UI 갱신)
        ↓ 경로 문자열 조합                           — 빠름
        ↓ CIniFileCS INI(strSWModelBase)            — 빠름 (경로 저장만)
        ↓ INI.Get_String("MODEL NAME")              — ★ 디스크 I/O
        ↓ INI.Get_Integer("VERSION")                — ★ 디스크 I/O
        ↓ INI.Get_Integer("TrayArrayX")             — ★ 디스크 I/O
        ↓ INI.Get_Integer("TrayArrayY")             — ★ 디스크 I/O
        ↓ INI.Get_Integer("TrayLotModuleAmt")       — ★ 디스크 I/O
        ↓ UpdateData(FALSE)                         — 빠름 (UI 갱신)
```

---

## 2. 병목 지점 분석

### [주요] 문제 1: `GetPrivateProfile*` 5회 독립 호출 — 반복 디스크 I/O

`CIniFileCS`는 생성자에서 파일 경로만 저장하고, 내용을 캐시하지 않는다.  
각 `Get_*` 호출이 독립적인 Win32 `GetPrivateProfile*` API를 호출하며,  
**이 API는 매 호출마다 파일을 열고 읽고 닫는다.**

```cpp
// IniFileCS.cpp
int CIniFileCS::Get_Integer(CString strApp, CString strKey, int nDefault)
{
    return GetPrivateProfileInt(strApp, strKey, nDefault, m_strFile);  // 매번 파일 열기
}

CString CIniFileCS::Get_String(CString strApp, CString strKey, CString strDefault)
{
    char cReturn[1024];
    for (int i = 0; i < 1024; i++) cReturn[i] = 0;  // 하단 문제 4 참고
    GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);  // 매번 파일 열기
    ...
}
```

**예상 지연**: 로컬 SSD 기준 1회 약 1~5ms.  
5회 × 2~5ms = **10~25ms / 선택 이벤트**.  
HDD 또는 네트워크 드라이브: 1회 10~50ms → **최대 250ms 이상**.

---

### [주요] 문제 2: UI 스레드에서 동기 디스크 I/O

`OnSelchangeListModelnames()`는 `LBN_SELCHANGE` 메시지 핸들러이므로 **UI 스레드**에서 실행된다.  
디스크 I/O가 완료될 때까지 UI가 응답하지 않는다 (프리징).  
사용자가 키보드로 리스트를 빠르게 스크롤하면 이 함수가 연속으로 호출된다.

---

### [주요] 문제 3: 파일 존재 여부 확인 없이 읽기 시도

```cpp
// LoadModelDlg.cpp
CIniFileCS INI(strSWModelBase);         // 파일이 없어도 오류 없이 생성됨
strModelName = INI.Get_String(...);     // 파일 없으면 Win32 API가 파일 시스템 탐색 후 기본값 반환
```

파일이 없을 때도 Windows는 파일 시스템을 탐색한 뒤 기본값을 반환하므로 추가 오버헤드가 발생한다.

---

### [부수적] 문제 4: `Get_String` 내 수동 배열 초기화

```cpp
// IniFileCS.cpp:125-126
char cReturn[1024];
for (int i = 0; i < 1024; i++)
    cReturn[i] = 0;   // memset 으로 대체해야 함
```

`memset(cReturn, 0, sizeof(cReturn))` 으로 대체하는 것이 관례다.  
영향은 미미하지만, 최적화 수준에 따라 루프가 그대로 남을 수 있다.

---

### [부수적] 문제 5: 선택 이벤트마다 전체 읽기 트리거

`LBN_SELCHANGE`는 마우스 클릭뿐 아니라 **키보드 화살표 이동**에도 발생한다.  
↑↓ 키를 빠르게 누르면 모델 수만큼 INI 파일 I/O가 연속 발생한다.

---

## 3. 병목 위치 요약 (우선순위 순)

| 순위 | 위치 | 원인 | 예상 지연 |
|---|---|---|---|
| 1 | `INI.Get_String / Get_Integer` × 5 | Win32 INI API 매 호출마다 파일 재열기 | 10~250ms |
| 2 | `OnSelchangeListModelnames` 전체 | UI 스레드에서 동기 I/O 실행 | I/O 지연과 동일 |
| 3 | 파일 존재 미확인 | 없는 파일도 탐색 시도 | 수 ms |
| 4 | `Get_String` 수동 배열 초기화 | `for` 루프 대신 `memset` 필요 | < 0.1ms |
| 5 | 키보드 스크롤 시 연속 호출 | `LBN_SELCHANGE` 이벤트 특성 | 위 × N |

---

## 4. 개선 방향 제안

### 단기 개선 (즉시 적용 가능)

**A. `GetPrivateProfileSection`으로 1회 읽기로 통합**

```cpp
// 현재: 5회 개별 호출 (5× 파일 열기)
strModelName          = INI.Get_String("Model Base", "MODEL NAME", "DEFAULT");
iCurrent_ModelVersion = INI.Get_Integer("Model Base", "VERSION", 1000);
iCurrent_TrayArrayX   = INI.Get_Integer("Model Base", "TrayArrayX", 3);
...

// 개선: 섹션 전체를 1회 읽어 메모리에서 파싱
TCHAR szBuffer[4096] = {0};
GetPrivateProfileSection(_T("Model Base"), szBuffer, 4096, strSWModelBase);
// szBuffer: "KEY=VALUE\0KEY=VALUE\0\0" 형식 → 직접 파싱
```

**B. 파일 존재 여부 선행 확인**

```cpp
if (!PathFileExists(strSWModelBase))
{
    m_EditModelDescription = _T("(ModelBase.ini not found)");
    UpdateData(FALSE);
    return;
}
```

**C. `Get_String` 수동 초기화 → `memset` 대체**  
`IniFileCS.cpp` 내 모든 `for (int i = 0; i < N; i++) arr[i] = 0;` 패턴 일괄 교체.

---

### 중기 개선 (구조 변경 필요)

**D. `CIniFileCS`에 파일 내용 캐싱 추가**

생성자에서 파일 전체를 한 번 읽어 `std::map<CString, CString>` 에 파싱 후 저장.  
이후 `Get_*` 호출은 메모리에서 조회 → 파일 I/O 제로.

**E. 비동기 읽기 (워커 스레드 분리)**

`OnSelchangeListModelnames`에서 즉시 빈 설명을 표시하고,  
워커 스레드에서 INI 읽기 후 `PostMessage`로 UI 갱신.  
UI 응답성 완전 보장.

**F. 디바운싱 적용**

키보드 연속 스크롤 시 마지막 이벤트만 처리:

```cpp
// 300ms 후에도 선택이 바뀌지 않으면 읽기 실행
KillTimer(TIMER_MODEL_PREVIEW);
SetTimer(TIMER_MODEL_PREVIEW, 300, nullptr);
// OnTimer 에서 실제 INI 읽기 수행
```

---

## 5. 관련 파일 목록

| 파일 | 관련 내용 |
|---|---|
| `LoadModelDlg.cpp` | `OnSelchangeListModelnames()` — 분석 대상 함수 |
| `IniFileCS.cpp / .h` | `CIniFileCS::Get_*` — 매 호출마다 Win32 INI API 직접 호출 |
| `ModelDataManager.h` | `GetModelIdx()` — 인라인 getter, 오버헤드 없음 |
| `uScan.h` | `GetWorkingDirectory()` — 인라인 getter, 오버헤드 없음 |
