# HALCON C++ API 함수 레퍼런스

> **버전**: HALCON 10.0 (MVTec Software GmbH)
> **헤더 경로**: `C:\Program Files\MVTec\HALCON-10.0\include\cpp\`
> **링크 라이브러리**: `halconcppxl.lib`
> **네임스페이스**: `Halcon::`
> **작성일**: 2026-03-11

---

## 목차

1. [핵심 데이터 타입](#1-핵심-데이터-타입)
2. [HImage — 이미지 처리](#2-himage--이미지-처리)
3. [HRegion — 영역(Blob) 처리](#3-hregion--영역blob-처리)
4. [HXLDCont — XLD 컨투어 처리](#4-hxldcont--xld-컨투어-처리)
5. [HMeasure — 측정 도구](#5-hmeasure--측정-도구)
6. [HShapeModel — 형상 매칭](#6-hshapemodel--형상-매칭)
7. [HNCCModel — NCC 템플릿 매칭](#7-hnccmodel--ncc-템플릿-매칭)
8. [HOperator — 동적 연산자 실행기](#8-hoperator--동적-연산자-실행기)
9. [전역(Global) HALCON 함수](#9-전역global-halcon-함수)
10. [HTuple — 범용 데이터 컨테이너](#10-htuple--범용-데이터-컨테이너)

---

## 1. 핵심 데이터 타입

| 타입 | 헤더 | 설명 |
|------|------|------|
| `HObject` | `HInterfaceCPP.h` | 모든 아이코닉 객체의 기반 클래스. `HImage`, `HRegion`, `HXLD` 등의 부모 |
| `HImage` | `HImage.h` | 1채널 또는 멀티채널 이미지 |
| `HRegion` | `HRegion.h` | 픽셀 집합으로 정의된 영역 (run-length 인코딩) |
| `HXLD` | `HXLD.h` | XLD(eXtended Line Description) 기반 클래스 |
| `HXLDCont` | `HXLDCont.h` | Sub-pixel 정밀도의 컨투어 (선분, 원호, 경계선) |
| `HXLDContArray` | `HXLDContArray.h` | `HXLDCont` 배열 |
| `HRegionArray` | `HRegionArray.h` | `HRegion` 배열 |
| `HImageArray` | `HImageArray.h` | `HImage` 배열 |
| `HTuple` | `HTuple.h` | 범용 값 컨테이너 (int, double, string 혼합 가능) |
| `HShapeModel` | `HShapeModel.h` | 형상 매칭 모델 핸들 |
| `HNCCModel` | `HNCCModel.h` | NCC 매칭 모델 핸들 |
| `HMeasure` | `HMeasure.h` | 1D 측정 도구 핸들 (엣지 검출용 ROI) |
| `Hlong` | `HIntDef.h` | HALCON 정수형 (32/64-bit 호환) |
| `Herror` | `HBase.h` | HALCON 에러 코드 타입 |

---

## 2. HImage — 이미지 처리

`HImage.h` | 클래스: `Halcon::HImage : public HObject`

### 2.1 생성자

```cpp
HImage()                                        // 빈 이미지
HImage(const char *file)                        // 파일에서 이미지 로드
HImage(int width, int height, const char *type) // 크기/타입 지정 생성
HImage(void *ptr, int width, int height, const char *type) // 외부 메모리에서 생성
```

### 2.2 픽셀 접근

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `GetGrayval` | `HTuple GetGrayval(Hlong Row, Hlong Column) const` | 지정 좌표의 gray 값 읽기 |
| `GetGrayvalInterpolated` | `HTuple GetGrayvalInterpolated(double Row, double Col, const char *Interpolation) const` | 보간을 이용한 sub-pixel gray 값 읽기 |
| `SetGrayval` | `void SetGrayval(Hlong Row, Hlong Column, double Grayval) const` | gray 값 설정 |
| `GetImagePointer1` | `Hlong GetImagePointer1(HTuple *Type, HTuple *Width, HTuple *Height) const` | 이미지 데이터 포인터 및 메타데이터 획득 |
| `GetImagePointer3` | `Hlong GetImagePointer3(HTuple *PtrGreen, HTuple *PtrBlue, ...) const` | RGB 채널별 포인터 획득 |
| `GetImageType` | `HTuple GetImageType(void) const` | 이미지 픽셀 타입 반환 (`"byte"`, `"real"`, `"int2"` 등) |
| `GetImageSize` | `Hlong GetImageSize(Hlong *Height) const` | 너비(반환값)/높이(포인터) 획득 |
| `Width` / `Height` | `int Width(void) const` | 이미지 크기 |

### 2.3 이미지 생성 (static)

| 함수 | 설명 |
|------|------|
| `GenImageConst(const char *Type, Hlong Width, Hlong Height)` | 지정 타입의 빈(0) 이미지 생성 |
| `GenImage1(const char *Type, Hlong W, Hlong H, Hlong Ptr)` | 외부 포인터에서 1채널 이미지 생성 |
| `GenImage3(const char *Type, Hlong W, Hlong H, Hlong R, Hlong G, Hlong B)` | RGB 3채널 이미지 생성 |
| `GenImageInterleaved(...)` | Interleaved 픽셀 데이터에서 이미지 생성 |
| `GenImageGrayRamp(...)` | 그레이 램프(기울기) 이미지 생성 |

### 2.4 이미지 변환 및 복사

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `CopyImage` | `HImage CopyImage(void) const` | 이미지 메모리 복사 |
| `ConvertImageType` | (전역 함수) | 픽셀 타입 변환 (`byte`↔`real` 등) |
| `ScaleImage` | (전역 함수) | gray 값 선형 스케일링 |
| `PaintGray` | `HImage PaintGray(const HImage &Dest) const` | gray 값을 다른 이미지에 복사 |
| `OverpaintGray` | `void OverpaintGray(const HImage &Src) const` | 소스 이미지의 gray 값으로 덮어쓰기 |
| `OverpaintRegion` | `void OverpaintRegion(const HRegion &Rgn, double Gray, const char *Type) const` | 영역을 지정 gray 값으로 칠하기 |
| `GenImageProto` | `HImage GenImageProto(double Grayval) const` | 동일 크기의 단색 이미지 생성 |
| `CropDomainRel` | `HImage CropDomainRel(Hlong Top, Hlong Left, Hlong Bottom, Hlong Right) const` | domain 기준 상대 위치로 이미지 자르기 |

### 2.5 세그멘테이션 (임계화)

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `Threshold` | `HRegion Threshold(double MinGray, double MaxGray) const` | 전역 임계값으로 영역 추출 |
| `ThresholdSubPix` | `HXLDContArray ThresholdSubPix(double Threshold) const` | Sub-pixel 정밀도 레벨 크로싱 추출 |
| `DynThreshold` | `HRegion DynThreshold(const HImage &ThreshImg, double Offset, const char *LightDark) const` | 로컬 임계값 기반 세그멘테이션 |
| `VarThreshold` | `HRegion VarThreshold(Hlong MaskW, Hlong MaskH, double StdScale, double AbsThresh, const char *Mode) const` | 평균/표준편차 기반 적응 임계화 |
| `AutoThreshold` | `HRegionArray AutoThreshold(double Sigma) const` | 히스토그램에서 임계값 자동 결정 |
| `BinThreshold` | `HRegion BinThreshold(void) const` | 자동 이진화 |
| `FastThreshold` | `HRegion FastThreshold(double MinGray, double MaxGray, Hlong MinSize) const` | 빠른 전역 임계화 (최소 크기 필터 포함) |
| `HysteresisThreshold` | `HRegion HysteresisThreshold(Hlong Low, Hlong High, Hlong MaxLen) const` | 이력 임계화 (약한 엣지 연결) |
| `CharThreshold` | `HRegion CharThreshold(const HRegion &HistoRgn, double Sigma, double Percent, Hlong *Threshold) const` | 문자 추출용 임계화 |

### 2.6 에지 검출

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `EdgesSubPix` | (전역 함수) | Sub-pixel 정밀도 엣지 검출 → `HXLDContArray` 반환 |
| `ZeroCrossing` | `HRegion ZeroCrossing(void) const` | 이미지의 영점 교차(zero crossing) 추출 |
| `ZeroCrossingSubPix` | `HXLDContArray ZeroCrossingSubPix(void) const` | Sub-pixel 영점 교차 추출 |
| `LocalMin` | `HRegion LocalMin(void) const` | 로컬 최솟값 위치 검출 |
| `LocalMax` | `HRegion LocalMax(void) const` | 로컬 최댓값 위치 검출 |
| `NonmaxSuppressionAmp` | `HImage NonmaxSuppressionAmp(const char *Mode) const` | non-maximum suppression (엣지 세선화) |
| `NonmaxSuppressionDir` | `HImage NonmaxSuppressionDir(const HImage &ImgDir, const char *Mode) const` | 방향 이미지를 이용한 non-maximum suppression |

### 2.7 형태학적 연산 (Gray Morphology)

> 주의: 아래는 **gray morphology** 함수로, `HImage` 멤버입니다.
> **Region morphology** (`Erosion`, `Dilation`, `Opening`, `Closing`)는 `HRegion` 또는 전역 함수에서 사용합니다.

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `GrayErosion` | `HImage GrayErosion(const HImage &SE) const` | gray 침식 (SE는 구조요소 이미지) |
| `GrayDilation` | `HImage GrayDilation(const HImage &SE) const` | gray 팽창 |
| `GrayOpening` | `HImage GrayOpening(const HImage &SE) const` | gray Opening (침식→팽창) |
| `GrayClosing` | `HImage GrayClosing(const HImage &SE) const` | gray Closing (팽창→침식) |
| `GrayTophat` | `HImage GrayTophat(const HImage &SE) const` | gray Top-hat 변환 |
| `GrayBothat` | `HImage GrayBothat(const HImage &SE) const` | gray Bottom-hat 변환 |
| `GenDiscSe` (static) | `static HImage GenDiscSe(const char *Type, Hlong W, Hlong H, double Smax)` | 타원형 구조요소 생성 |

### 2.8 영역 분할

| 함수 | 설명 |
|------|------|
| `Regiongrowing(Hlong Row, Hlong Column, double Tolerance, Hlong MinSize)` | 지정 시드점부터 gray 차이 기반 영역 성장 |
| `RegiongrowingMean(...)` | 평균 gray 기반 영역 성장 |
| `Watersheds(HRegion *Watersheds)` | 워터셰드 분할 (분수계 + 분지 반환) |
| `WatershedsThreshold(Hlong Threshold)` | 임계값 기반 워터셰드 분지 추출 |
| `Pouring(const char *Mode, Hlong MinGray, Hlong MaxGray)` | 물 따르기 비유의 영역 분할 |

### 2.9 형상 매칭 (HImage → HShapeModel)

| 함수 | 설명 |
|------|------|
| `CreateShapeModel(Hlong NumLevels, double AngleStart, double AngleExtent, double AngleStep, ...)` | 이미지에서 형상 모델 생성 |
| `FindShapeModel(const HShapeModel &ModelID, double AngleStart, ..., Halcon::HTuple *Row, ...)` | 형상 모델 탐색 (위치·각도·점수 반환) |
| `CreateNccModel(...)` | NCC 모델 생성 |
| `FindNccModel(...)` | NCC 모델 탐색 |

### 2.10 1D 측정 (HImage에서 MeasurePos 사용)

```cpp
// HMeasure 객체를 먼저 생성한 뒤 HImage에 적용
HMeasure measure(Row, Column, Phi, Length1, Length2, Width, Height, "nearest_neighbor");
HTuple rowEdge = Image.MeasurePos(measure, Sigma, Threshold, "positive", "all", &ColEdge, &Amplitude, &Distance);
```

---

## 3. HRegion — 영역(Blob) 처리

`HRegion.h` | 클래스: `Halcon::HRegion : public HAbstractRegion, HObject`

### 3.1 영역 생성 (static)

| 함수 | 설명 |
|------|------|
| `GenRectangle1(double R1, double C1, double R2, double C2)` | 축 정렬 직사각형 생성 |
| `GenRectangle2(double Row, double Col, double Phi, double L1, double L2)` | 임의 방향 직사각형 생성 |
| `GenRegionPoints(Hlong Rows, Hlong Columns)` | 개별 픽셀 집합에서 영역 생성 |
| `GenRegionPolygon(const HTuple &Rows, const HTuple &Columns)` | 폴리곤 윤곽선 영역 생성 |
| `GenRegionPolygonFilled(...)` | 폴리곤 내부 채움 영역 생성 |
| `GenRegionRuns(Hlong Row, Hlong ColBegin, Hlong ColEnd)` | Run-length 코딩으로 영역 생성 |

### 3.2 영역 집합 연산

| 함수 / 연산자 | 설명 |
|------|------|
| `Union2(const HRegion &Reg2)` | 두 영역의 합집합 (`operator |`) |
| `Intersection(const HRegion &Reg2)` | 두 영역의 교집합 (`operator &`) |
| `Difference(const HRegion &Sub)` | 차집합 (`operator -`) |
| `Complement(void)` | 보집합 (이미지 전체 - 영역) (`operator ~`) |
| `SymmDifference(const HRegion &Reg2)` | 대칭 차집합 |

### 3.3 형태학적 연산 (Region Morphology)

> **전역 함수** `Erosion`, `Dilation`, `Opening`, `Closing`을 사용하거나,
> **HRegion 인픽스 연산자**를 사용합니다:

| 연산자/함수 | 설명 |
|------|------|
| `operator >> (double radius)` | 팽창(Dilation) — radius만큼 팽창 |
| `operator << (double radius)` | 침식(Erosion) — radius만큼 침식 |
| `FillUp(void)` | 내부 구멍(hole) 채우기 |
| `FillUpShape(const char *Feature, double Min, double Max)` | 특정 형상 조건의 구멍만 채우기 |
| `Skeleton(void)` | 스켈레톤 추출 |
| `RemoveNoiseRegion(const char *Type)` | 노이즈 영역 제거 |
| `ShapeTrans(const char *Type)` | 형상 변환 (`"convex"`, `"ellipse"`, `"rectangle1"` 등) |

### 3.4 연결 요소 분리

```cpp
HRegionArray Connection(void) const   // 연결된 컴포넌트 분리 (4/8-connectivity)
```

### 3.5 영역 선택 (HRegionArray 수준)

> `Connection()` 결과인 `HRegionArray`에서 특징 기반으로 선택:

```cpp
// 전역 함수 사용
HRegionArray SelectShape(const HRegionArray &Regions,
                          const HTuple &Features,   // "area", "width", "height", "row", "column" 등
                          const char   *Operation,  // "and", "or"
                          const HTuple &Min,
                          const HTuple &Max);

HObject SelectObj(const HObject &Objects, Hlong Index);  // 인덱스로 단일 객체 선택
```

### 3.6 영역 기하 변환

| 함수 | 설명 |
|------|------|
| `MoveRegion(Hlong Row, Hlong Column)` | 영역 이동(평행 이동) |
| `ZoomRegion(double ScaleW, double ScaleH)` | 영역 확대/축소 |
| `AffineTransRegion(const HTuple &HomMat2D, const char *Interpolate)` | 어파인 변환 |
| `MirrorRegion(const char *Mode, Hlong WidthHeight)` | 반사 변환 |
| `ClipRegion(Hlong R1, Hlong C1, Hlong R2, Hlong C2)` | 직사각형 영역으로 클리핑 |
| `ClipRegionRel(Hlong Top, Hlong Bottom, Hlong Left, Hlong Right)` | 크기 기준 상대 클리핑 |
| `ExpandRegion(const HRegion &Forbidden, Hlong Iterations, const char *Mode)` | 금지 영역 제외 확장 |

### 3.7 영역 특징 측정

| 함수 | 반환 타입 | 설명 |
|------|---------|------|
| `Area(void)` | `Hlong` | 면적 (픽셀 수) |
| `Row(void)` / `Col(void)` | `double` | 무게중심 행/열 좌표 |
| `SmallestRectangle1(...)` | `void` (출력 파라미터) | 축 정렬 최소 외접 직사각형 (R1, C1, R2, C2) |
| `SmallestRectangle2(...)` | `void` (출력 파라미터) | 임의 방향 최소 외접 직사각형 (Row, Col, Phi, L1, L2) |
| `Anisometry(void)` | `double` | 타원 비율 (Ra/Rb) |
| `M11()` / `M20()` / `M02()` | `double` | 2차 모멘트 |
| `Phi(void)` | `double` | 주축 방향 각도 |
| `Ra(void)` / `Rb(void)` | `double` | 주/부 축 반지름 |

> 전역 함수로도 사용 가능:
> ```cpp
> AreaCenter(Region, &Area, &Row, &Column);
> SmallestRectangle1(Regions, &Row1, &Column1, &Row2, &Column2);
> SmallestRectangle2(Regions, &Row, &Column, &Phi, &Length1, &Length2);
> ```

### 3.8 영역 ↔ XLD/Image 변환

| 함수 | 설명 |
|------|------|
| `GenContourRegionXld(const char *Mode)` | 영역 경계를 XLD 컨투어로 변환 (`"border"`, `"center"`) |
| `GenContoursSkeletonXld(Hlong Length, const char *Mode)` | 스켈레톤을 XLD 컨투어로 변환 |
| `RegionToLabel(const char *Type, Hlong W, Hlong H)` | 영역 배열을 레이블 이미지로 변환 |
| `RegionToBin(Hlong FgGray, Hlong BgGray, Hlong W, Hlong H)` | 영역을 이진 이미지로 변환 |
| `PaintRegion(const HImage &Img, double Gray, const char *Type)` | 영역을 이미지 위에 그리기 |

### 3.9 테스트/검증

| 함수 | 설명 |
|------|------|
| `TestSubsetRegion(const HRegion &Reg2)` | Reg2가 this 안에 포함되는지 확인 |
| `TestEqualRegion(const HRegion &Reg2)` | 두 영역이 동일한지 확인 |
| (전역) `ValidHObject(obj)` | 객체가 유효한지 확인 |

---

## 4. HXLDCont — XLD 컨투어 처리

`HXLDCont.h` | 클래스: `Halcon::HXLDCont : public HXLD`

> **XLD (eXtended Line Description)**: sub-pixel 정밀도의 선분/경계 표현.
> 각 포인트는 실수 좌표 `(Row, Col)` + 속성값(Amplitude 등)을 가짐.

### 4.1 컨투어 생성 (static)

| 함수 | 설명 |
|------|------|
| `GenCrossContourXld(double Row, double Col, double Size, double Angle)` | 십자(+) 마커 생성 |
| `GenCircleContourXld(double Row, double Col, double Radius, double StartPhi, double EndPhi, const char *PointOrder, double Resolution)` | 원 또는 원호 컨투어 생성 |
| `GenEllipseContourXld(double Row, double Col, double Phi, double R1, double R2, double StartPhi, double EndPhi, const char *PointOrder, double Resolution)` | 타원 또는 타원호 컨투어 생성 |
| `GenRectangle2ContourXld(double Row, double Col, double Phi, double L1, double L2)` | 직사각형 컨투어 생성 |
| `GenContourPolygonXld(const HTuple &Rows, const HTuple &Cols)` | 폴리곤에서 컨투어 생성 |
| `GenContourPolygonRoundedXld(const HTuple &Rows, const HTuple &Cols, const HTuple &Radius, ...)` | 모서리 둥근 폴리곤 컨투어 생성 |
| `GenContourNurbsXld(...)` | NURBS 커브에서 컨투어 생성 |

### 4.2 피팅(Fitting)

| 함수 | 반환 | 주요 파라미터 | 설명 |
|------|------|--------------|------|
| `FitLineContourXld(const char *Algorithm, Hlong MaxNumPoints, Hlong ClippingEndPoints, Hlong Iterations, double ClippingFactor, double *RowBegin, double *ColBegin, double *RowEnd, double *ColEnd, double *Nr, double *Nc, double *Dist)` | `double` (RowBegin) | Algorithm: `"regression"`, `"huber"`, `"tukey"`, `"drop"` | 직선 피팅 → 시작/끝점 + 법선벡터 + 거리 |
| `FitCircleContourXld(const char *Algorithm, Hlong MaxNumPoints, double MaxClosureDist, Hlong ClippingEndPoints, Hlong Iterations, double ClippingFactor, double *Column, double *Radius, double *StartPhi, double *EndPhi, char *PointOrder)` | `double` (Row) | Algorithm: `"algebraic"`, `"ahuber"`, `"atukey"` | 원 피팅 → 중심(Row,Col) + 반지름 + 원호 범위 |
| `FitEllipseContourXld(const char *Algorithm, ...)` | `double` | | 타원 피팅 → 중심 + Phi + 반지름 1/2 + 원호 범위 |
| `FitRectangle2ContourXld(const char *Algorithm, ...)` | `double` | | 직사각형 피팅 |

### 4.3 컨투어 데이터 접근

| 함수 | 반환 | 설명 |
|------|------|------|
| `GetContourXld(HTuple *Col)` | `HTuple` (Row) | 컨투어의 모든 Row/Col 좌표 추출 |
| `ContourPointNumXld(void)` | `Hlong` | 컨투어 포인트 개수 |
| `GetContourAngleXld(const char *AngleMode, const char *CalcMode, Hlong Lookaround)` | `HTuple` | 각 포인트의 컨투어 방향 각도 |
| `GetContourAttribXld(const char *Name)` | `HTuple` | 포인트별 속성값 (예: `"edge_direction"`, `"amplitude"`) |
| `GetContourGlobalAttribXld(const char *Name)` | `HTuple` | 컨투어 전체 속성값 |

### 4.4 거리 계산

| 함수 | 설명 |
|------|------|
| `DistancePc(double Row, double Col, HTuple *DistanceMax)` | 점과 컨투어 간 최소/최대 거리 |
| `DistanceLc(double R1, double C1, double R2, double C2, HTuple *DistMax)` | 직선과 컨투어 간 거리 |
| `DistanceSc(double R1, double C1, double R2, double C2, HTuple *DistMax)` | 선분과 컨투어 간 거리 |
| `DistanceCc(const HXLDCont &Cont2, const char *Mode, double *DistMax)` | 두 컨투어 간 거리 |
| `DistanceCcMin(const HXLDCont &Cont2, const char *Mode)` | 두 컨투어 간 최소 거리 |
| `DistEllipseContourXld(const char *Mode, ...)` | 타원까지의 거리 |

### 4.5 컨투어 변환 및 가공

| 함수 | 설명 |
|------|------|
| `AffineTransContourXld(const HTuple &HomMat2D)` | 어파인 변환 |
| `ProjectiveTransContourXld(const HTuple &HomMat2D)` | 투영 변환 |
| `ClipContoursXld(Hlong R1, Hlong C1, Hlong R2, Hlong C2)` | 직사각형 영역으로 클리핑 |
| `CropContoursXld(double R1, double C1, double R2, double C2, const char *CloseContours)` | 컨투어 자르기 |
| `ClipEndPointsContoursXld(const char *Mode, double Length)` | 끝점 잘라내기 |
| `SegmentContoursXld(const char *Mode, Hlong SmoothCont, double MaxLineDist1, double MaxLineDist2)` | 직선/원호 세그먼트로 분리 |
| `PolarTransContourXld(...)` | 극좌표계로 변환 |
| `PolarTransContourXldInv(...)` | 극좌표계에서 직교좌표계로 역변환 |

### 4.6 XLD ↔ Region 변환

| 함수 | 설명 |
|------|------|
| `GenRegionContourXld(const char *Mode)` | 컨투어를 영역으로 변환 (`"filled"`, `"border"`) |
| `CreateShapeModelXld(...)` | XLD에서 형상 모델 생성 |

### 4.7 집합 연산 (closed contours)

| 함수 | 설명 |
|------|------|
| `Union2ClosedContoursXld(const HXLDCont &Cont2)` | 닫힌 컨투어 합집합 |
| `IntersectionClosedContoursXld(const HXLDCont &Cont2)` | 닫힌 컨투어 교집합 |
| `DifferenceClosedContoursXld(const HXLDCont &Sub)` | 닫힌 컨투어 차집합 |

---

## 5. HMeasure — 측정 도구

`HMeasure.h` | 클래스: `Halcon::HMeasure : public HToolBase`

> **1D 측정**: 직사각형 또는 원호 ROI를 따라 수직 방향 엣지를 고정밀도로 검출.

### 5.1 생성자 (측정 ROI 설정)

```cpp
// 직사각형 ROI
HMeasure(double Row, double Column, double Phi,
         double Length1, double Length2,
         Hlong Width, Hlong Height,
         const char *Interpolation);  // "nearest_neighbor", "bilinear", "bicubic"

// 원호 ROI
HMeasure(double CenterRow, double CenterCol, double Radius,
         double AngleStart, double AngleExtent, double AnnulusRadius,
         Hlong Width, Hlong Height,
         const char *Interpolation);
```

### 5.2 ROI 생성 메서드

| 함수 | 설명 |
|------|------|
| `GenMeasureRectangle2(double Row, double Col, double Phi, double L1, double L2, Hlong W, Hlong H, const char *Interp)` | 직사각형 ROI 생성/갱신 |
| `GenMeasureArc(double CenterRow, double CenterCol, double Radius, double AngleStart, double AngleExtent, double AnnulusRadius, ...)` | 원호 ROI 생성/갱신 |
| `TranslateMeasure(double Row, double Column)` | ROI 이동 |

### 5.3 엣지 검출

| 함수 | 주요 파라미터 | 출력 | 설명 |
|------|-------------|------|------|
| `MeasurePos(Image, double Sigma, double Threshold, const char *Transition, const char *Select, HTuple *ColEdge, HTuple *Amplitude, HTuple *Distance)` | Transition: `"positive"`, `"negative"`, `"all"` | Row of edges | 단일 엣지 위치 검출 |
| `MeasurePairs(Image, Sigma, Threshold, Transition, Select, ...)` | | Row of 1st edge | 엣지 쌍(폭 측정) 검출 |
| `MeasureProjection(Image)` | — | 프로파일 (HTuple) | ROI 방향의 gray 프로파일 추출 |
| `MeasureThresh(Image, Sigma, Threshold, Select, ...)` | | 특정 gray 임계 위치 | 임계값 위치 검출 |

### 5.4 퍼지 측정 (Fuzzy Measure)

```cpp
// 퍼지 멤버십 함수 설정 후 사용
SetFuzzyMeasure(SetType, Function)        // 퍼지 함수 설정
FuzzyMeasurePos(Image, Sigma, AmpThresh, FuzzyThresh, Transition, ...)  // 퍼지 엣지 검출
FuzzyMeasurePairs(...)                    // 퍼지 엣지 쌍 검출
FuzzyMeasurePairing(...)                  // 지정된 개수의 엣지 쌍 매칭
```

---

## 6. HShapeModel — 형상 매칭

`HShapeModel.h` | 클래스: `Halcon::HShapeModel : public HToolBase`

### 6.1 모델 생성

```cpp
// 이미지 템플릿에서 생성 (생성자에서 자동으로 CreateShapeModel 호출)
HShapeModel model(TemplateImage,
                  NumLevels,    // 피라미드 레벨 수
                  AngleStart,   // 탐색 각도 시작 (rad)
                  AngleExtent,  // 탐색 각도 범위 (rad)
                  AngleStep,    // 각도 해상도 (rad)
                  "none",       // Optimization: "none", "point_reduction_low/medium/high"
                  "use_polarity", // Metric
                  30,           // Contrast (최소 대비)
                  10);          // MinContrast
```

| 함수 | 설명 |
|------|------|
| `CreateShapeModel(Template, NumLevels, AngleStart, AngleExtent, AngleStep, ...)` | 회전 불변 모델 생성 |
| `CreateScaledShapeModel(Template, ..., ScaleMin, ScaleMax, ScaleStep, ...)` | 스케일 불변 모델 생성 |
| `CreateAnisoShapeModel(Template, ..., ScaleRMin, ScaleRMax, ScaleRStep, ScaleCMin, ...)` | 비등방 스케일 불변 모델 생성 |
| `CreateShapeModelXld(Contours, ...)` | XLD 컨투어에서 모델 생성 |

### 6.2 탐색

| 함수 | 주요 출력 | 설명 |
|------|---------|------|
| `FindShapeModel(Image, AngleStart, AngleExtent, MinScore, NumMatches, MaxOverlap, SubPixel, NumLevels, Greediness, *Column, *Angle, *Score)` | Row, Column, Angle, Score | 단일 모델 탐색 |
| `FindScaledShapeModel(Image, ..., ScaleMin, ScaleMax, ...)` | Row, Column, Angle, Scale, Score | 스케일 불변 탐색 |
| `FindAnisoShapeModel(Image, ..., ScaleRMin, ScaleRMax, ScaleCMin, ScaleCMax, ...)` | + ScaleR, ScaleC | 비등방 스케일 탐색 |
| `FindShapeModels(Images, ...)` | + Model (어떤 모델인지 인덱스) | 여러 이미지 동시 탐색 |

### 6.3 모델 관리

| 함수 | 설명 |
|------|------|
| `ReadShapeModel(const char *FileName)` | 파일에서 모델 로드 |
| `WriteShapeModel(const char *FileName)` | 모델을 파일에 저장 |
| `GetShapeModelContours(Hlong Level)` | 특정 레벨의 모델 컨투어 반환 |
| `GetShapeModelParams(...)` | 모델 파라미터 조회 |
| `SetShapeModelOrigin(double Row, double Column)` | 모델의 기준점(원점) 설정 |
| `GetShapeModelOrigin(double *Column)` | 기준점 조회 |
| `SetShapeModelParam(const HTuple &Names, const HTuple &Values)` | 모델 파라미터 수정 |

---

## 7. HNCCModel — NCC 템플릿 매칭

`HNCCModel.h` | 클래스: `Halcon::HNCCModel : public HToolBase`

> **NCC (Normalized Cross-Correlation)**: gray 값 상관관계 기반 템플릿 매칭.
> 형상 매칭보다 계산 비용이 낮지만 변형/스케일 변화에 취약.

### 7.1 모델 생성

```cpp
HNCCModel model(TemplateImage,
                NumLevels,    // 피라미드 레벨 수
                AngleStart,   // 탐색 각도 시작 (rad)
                AngleExtent,  // 탐색 각도 범위 (rad)
                AngleStep,    // 각도 해상도 (rad)
                "use_polarity"); // Metric
```

| 함수 | 설명 |
|------|------|
| `CreateNccModel(Template, NumLevels, AngleStart, AngleExtent, AngleStep, Metric)` | NCC 모델 생성 |
| `DetermineNccModelParams(Template, NumLevels, AngleStart, AngleExtent, Metric, Parameters, *Value)` | 최적 파라미터 자동 결정 |

### 7.2 탐색

```cpp
HTuple rowResult = model.FindNccModel(
    Image,
    AngleStart, AngleExtent,  // 탐색 각도 범위
    MinScore,                 // 최소 점수 (0.0~1.0)
    NumMatches,               // 최대 매칭 개수
    MaxOverlap,               // 최대 중첩 비율
    SubPixel,                 // "true" or "false"
    NumLevels,                // 탐색 피라미드 레벨
    &Column, &Angle, &Score);
```

### 7.3 모델 관리

| 함수 | 설명 |
|------|------|
| `ReadNccModel(const char *FileName)` | 파일에서 로드 |
| `WriteNccModel(const char *FileName)` | 파일에 저장 |
| `GetNccModelParams(...)` | 파라미터 조회 |
| `SetNccModelOrigin(double Row, double Column)` | 기준점 설정 |
| `SetNccModelParam(const HTuple &Names, const HTuple &Values)` | 파라미터 수정 |

---

## 8. HOperator — 동적 연산자 실행기

`HOperator.h` | 클래스: `Halcon::HOperator`

> 연산자 이름(문자열)으로 HALCON 함수를 동적으로 호출하는 유틸리티 클래스.
> 리플렉션이 필요한 경우(예: 파라미터 이름으로 접근)에 사용.

```cpp
HOperator op("threshold");
op.SetInputObject("Image", image);
op.SetInputCtrl("MinGray", HTuple(100));
op.SetInputCtrl("MaxGray", HTuple(200));
op.Execute();
HObject region = op.GetOutputObject("Region");
```

---

## 9. 전역(Global) HALCON 함수

HALCON C++ 인터페이스에서 멤버 함수 외에도 **전역 함수** 형태로 동일 연산을 호출할 수 있습니다.
(C API `HProto.h`를 감싼 래퍼로, `Halcon::` 네임스페이스에 속함)

### 9.1 객체 관리

| 함수 | 시그니처 | 설명 |
|------|----------|------|
| `ConcatObj` | `void ConcatObj(const HObject &O1, const HObject &O2, HObject *OConcat)` | 두 아이코닉 객체 배열 연결 |
| `GenEmptyObj` | `void GenEmptyObj(HObject *EmptyObject)` | 빈 객체 생성 (초기화용) |
| `CountObj` | `void CountObj(const HObject &Objects, Hlong *Number)` | 배열의 객체 개수 카운트 |
| `SelectObj` | `void SelectObj(const HObject &Objects, HObject *ObjSelected, Hlong Index)` | 인덱스로 객체 선택 |
| `CopyObj` | `void CopyObj(const HObject &Objects, HObject *ObjectSelected, Hlong Index, Hlong NumObj)` | 객체 복사 |

### 9.2 이미지 변환

| 함수 | 설명 |
|------|------|
| `ConvertImageType(Image, *ImageConverted, const char *NewType)` | 픽셀 타입 변환 (`"byte"`, `"real"`, `"int2"`, `"uint2"`, `"int4"` 등) |
| `ScaleImage(Image, *ImageScaled, double Mult, double Add)` | gray 값 선형 변환: `g' = g * Mult + Add` |
| `Rgb1ToGray(RGBImage, *GrayImage)` | RGB → 그레이스케일 변환 |
| `GrayImage(MultiChannelImage, *Image)` | 멀티채널 이미지에서 첫 채널(gray) 추출 |
| `TransformImage(Image, *ImageAffineTrans, const HTuple &HomMat2D, const char *Interpolation, ...)` | 어파인 변환으로 이미지 변환 |
| `AffineTransImage(Image, *ImageAffineTrans, const HTuple &HomMat2D, const char *Interpolation, const char *AdaptImageSize)` | 어파인 변환 이미지 |
| `MirrorImage(Image, *Mirrored, const char *Mode)` | 이미지 반전 (`"row"`, `"column"`, `"row_column"`) |

### 9.3 에지 검출

```cpp
EdgesSubPix(
    const HObject &Image,
    HObject       *Edges,          // 출력: HXLDContArray
    const char    *Filter,         // "canny", "deriche1", "deriche2", "shen", "lanser1", "lanser2"
    double         Alpha,          // 필터 강도
    double         Low,            // 낮은 임계값
    double         High);          // 높은 임계값
```

### 9.4 영역 형태학 연산

```cpp
Erosion1(Region, *ErodedRegion, const HRegion &StructElement, Hlong Iterations);
Dilation1(Region, *DilatedRegion, const HRegion &StructElement, Hlong Iterations);
Opening(Region, *OpenRegion, const HRegion &StructElement);
Closing(Region, *ClosedRegion, const HRegion &StructElement);

// 단순 크기 기반
ErosionCircle(Region, *ErodedRegion, double Radius);
DilationCircle(Region, *DilatedRegion, double Radius);
OpeningCircle(Region, *OpenedRegion, double Radius);
ClosingCircle(Region, *ClosedRegion, double Radius);
ErosionRectangle1(Region, *ErodedRegion, Hlong Width, Hlong Height);
DilationRectangle1(Region, *DilatedRegion, Hlong Width, Hlong Height);
```

### 9.5 영역 특징 계산

```cpp
AreaCenter(Region, *Area, *Row, *Column);
SmallestRectangle1(Region, *Row1, *Column1, *Row2, *Column2);
SmallestRectangle2(Region, *Row, *Column, *Phi, *Length1, *Length2);
RegionFeatures(Region, const char *Features, *Value);  // 임의 특징 추출
SelectShape(Regions, *SelectedRegions, const char *Features, const char *Op, double Min, double Max);
```

### 9.6 기하/수학 연산

| 함수 | 설명 |
|------|------|
| `DistancePp(double Row1, double Col1, double Row2, double Col2, double *Distance)` | 두 점 간 거리 |
| `DistancePl(double Row, double Col, double R1, double C1, double R2, double C2, double *Distance)` | 점과 직선 거리 |
| `DistancePls(double Row, double Col, double R1, double C1, double R2, double C2, double *Distance)` | 점과 선분 거리 |
| `TupleAngle(HTuple *Angle, double X1, double Y1, double X2, double Y2)` | 두 벡터가 이루는 각도 |
| `VectorAngle(double X1, double Y1, double X2, double Y2, double *Angle)` | 벡터 (X1,Y1)→(X2,Y2)의 각도 |
| `AngleLx(double Row1, double Col1, double Row2, double Col2, double *Angle)` | 직선의 x축 기준 각도 |
| `IntersectionLines(double R1, double C1, double R2, double C2, double R3, double C3, double R4, double C4, double *Row, double *Col, char *IsParallel)` | 두 직선의 교점 |
| `IntersectionLineContour(Contour, double R1, double C1, double R2, double C2, *RowIntersection, *ColIntersection)` | 직선과 컨투어의 교점 |

### 9.7 어파인 변환 행렬 연산

| 함수 | 설명 |
|------|------|
| `HomMat2dIdentity(*HomMat2D)` | 2D 단위 변환 행렬 생성 |
| `HomMat2dTranslate(HomMat2D, Tx, Ty, *HomMat2DTranslate)` | 평행 이동 추가 |
| `HomMat2dRotate(HomMat2D, Phi, Px, Py, *HomMat2DRotate)` | 회전 추가 |
| `HomMat2dScale(HomMat2D, Sx, Sy, Px, Py, *HomMat2DScale)` | 스케일 추가 |
| `AffineTransPoint2d(HomMat2D, Row, Col, *RowTrans, *ColTrans)` | 점에 어파인 변환 적용 |

### 9.8 XLD 관련 전역 함수

| 함수 | 설명 |
|------|------|
| `LengthXld(XLD, *Length)` | 컨투어 전체 길이 |
| `LengthXldLine(Contour, *Length)` | 피팅된 직선의 길이 |
| `SmoothContoursXld(Contours, *SmoothedContours, Hlong SmoothingSize)` | 컨투어 스무딩 |
| `SelectContoursXld(Contours, *SelectedContours, const char *Feature, double Min1, double Max1, double Min2, double Max2)` | 특징 기반 컨투어 선택 |
| `UnionCollinearContoursXld(Contours, *UnionContours, double MaxDist, double MaxOverlap, double MaxCosAngle, double MaxCosAngleEndPoints, const char *Mode)` | 인접 직선 컨투어 통합 |

---

## 10. HTuple — 범용 데이터 컨테이너

`HTuple.h` | 클래스: `Halcon::HTuple`

> HALCON 연산자의 제어 파라미터와 결과값을 담는 컨테이너.
> `int`, `double`, `const char*`, 또는 이들의 혼합 배열을 저장.

### 10.1 생성 및 초기화

```cpp
HTuple empty;              // 빈 튜플
HTuple intVal(42);         // 정수
HTuple dblVal(3.14);       // 실수
HTuple strVal("hello");    // 문자열

// 배열 형태
HTuple arr;
arr.Append(1.0);
arr.Append(2.0);
arr.Append(3.0);
```

### 10.2 주요 연산자 및 메서드

| 함수/연산자 | 설명 |
|------|------|
| `operator [](Hlong Index)` | 인덱스로 요소 접근 |
| `Length()` | 요소 개수 |
| `D()` / `L()` / `S()` | double / long / string으로 단일값 추출 |
| `DArr()` / `LArr()` / `SArr()` | 배열 포인터 반환 |
| `Append(const HTuple &T)` | 요소 추가 |
| `Type()` | 저장된 타입 반환 (`HTYPE_INTEGER`, `HTYPE_DOUBLE`, `HTYPE_STRING`) |
| `IsEmpty()` | 비어있는지 확인 |
| `Concat(const HTuple &T)` | 두 튜플 연결 |
| `Select(Hlong Index)` | 요소 추출 |
| `SelectRange(Hlong IndexFrom, Hlong IndexTo)` | 범위 추출 |
| `Find(const HTuple &Val)` | 값 검색 (인덱스 반환) |
| `Sort()` | 정렬 |
| `Uniq()` | 중복 제거 |
| `TupleMax()` / `TupleMin()` | 최대/최솟값 |
| `TupleMean()` / `TupleSum()` | 평균/합계 |

### 10.3 수학 연산

```cpp
HTuple a(5.0), b(3.0);
HTuple c = a + b;   // 더하기
HTuple d = a - b;   // 빼기
HTuple e = a * b;   // 곱하기
HTuple f = a / b;   // 나누기
HTuple g = TupleSqrt(a);  // 제곱근
HTuple h = TupleAtan2(y, x);  // atan2
```

---

## 부록: 주요 파라미터 문자열 참조

### 이미지 타입 (`ConvertImageType`, `GenImage*`)

| 값 | 설명 |
|----|------|
| `"byte"` | 8-bit unsigned (0~255) |
| `"int1"` | 8-bit signed (-128~127) |
| `"int2"` | 16-bit signed |
| `"uint2"` | 16-bit unsigned |
| `"int4"` | 32-bit signed |
| `"real"` | 32-bit float |
| `"complex"` | 64-bit complex |

### 보간법 (`Interpolation`)

| 값 | 설명 |
|----|------|
| `"nearest_neighbor"` | 최근접 (빠름, 품질 낮음) |
| `"bilinear"` | 쌍선형 (균형) |
| `"bicubic"` | 쌍3차 (느림, 품질 높음) |

### 형상 매칭 Metric

| 값 | 설명 |
|----|------|
| `"use_polarity"` | 에지 극성 고려 (기본값, 가장 정확) |
| `"ignore_local_polarity"` | 로컬 극성 무시 |
| `"ignore_global_polarity"` | 전체 극성 무시 |
| `"ignore_color"` | 컬러 무시 |

### SelectShape / RegionFeatures 특징 이름

| 값 | 설명 |
|----|------|
| `"area"` | 면적 (픽셀 수) |
| `"row"` / `"column"` | 무게중심 좌표 |
| `"width"` / `"height"` | 외접 직사각형 너비/높이 |
| `"phi"` | 주축 방향 각도 |
| `"ra"` / `"rb"` | 주/부 반지름 |
| `"anisometry"` | 비원형도 (Ra/Rb) |
| `"roundness"` | 원형도 (4π*Area/Perimeter²) |
| `"compactness"` | 압축도 |
| `"convexity"` | 볼록도 |
| `"holes_num"` | 구멍 개수 |
| `"max_diameter"` | 최대 지름 |
| `"perimeter"` | 둘레 |

---

## 참고 문서

- **HALCON 13.0 Operator Reference (context7)**: `/websites/mvtec_doc_halcon_13_en`
- **로컬 헤더 경로**: `C:\Program Files\MVTec\HALCON-10.0\include\cpp\`
- **프로젝트 HALCON 래퍼**: `GTRegion.h` / `GTRegion.cpp` — HObject/HRegion 편의 래퍼
- **GFunction.h** — HALCON 연산을 감싼 프로젝트 유틸 함수 (`gFunc.ValidHRegion()` 등)
