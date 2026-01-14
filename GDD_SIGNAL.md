# 게임 기획서 (Game Design Document)

## 1. 개요 (Overview)

### 1.1 게임 제목
- **프로젝트명:** SIGNAL
- **장르:** 3D 액션 탐험 / Roguelite
- **플랫폼:** PC (Windows)

### 1.2 기획 의도
플레이어가 드론을 조작하여 프로시저럴 생성된 시설을 탐험하며  
Signal을 수집하고 탈출하는 액션 탐험 게임을 경험하도록 한다.

### 1.3 핵심 재미 요소 (Core Fun)
- 드론 조작을 통한 자유로운 3D 이동
- 배터리 관리와 리스크 리워드 시스템
- 프로시저럴 생성 던전의 반복 플레이 가치
- Signal 수집과 탈출을 위한 전략적 판단

---

## 2. 게임 콘셉트 (Game Concept)

### 2.1 세계관
- 미래의 폐기된 시설을 배경으로 한 SF/호러 분위기
- Signal이라는 에너지 자원을 둘러싼 탐험과 생존

### 2.2 플레이어 역할
- 탐사용 드론을 조작하는 조작자
- 시설 내부를 탐험하며 Signal을 수집하고 탈출해야 함
- 배터리 관리가 생존의 핵심

---

## 3. 게임 시스템 (Game Systems)

### 3.1 기본 조작
| 입력 | 기능 |
|----|----|
| 이동 키 | 드론 3D 이동 (수평/수직) |
| 시야 조작 | 카메라 회전 |
| 조명 토글 | 드론 조명 켜기/끄기 |
| 스캔 | 주변 아이템 탐지 |
| 추출 | Signal 아이템 추출 |
| 탈출 | Exit 지점으로 탈출 |

### 3.2 전투 시스템
- 직접 전투보다는 회피와 스텔스 중심
- 적 AI는 Idle, Combat, Dead 상태를 가짐
- 추출 시 발생하는 소음으로 적이 반응할 수 있음

### 3.3 성장 시스템
- Run 기반 시스템 (Roguelite)
- 각 스테이지에서 Signal을 수집하여 목표량 달성
- 스테이지 클리어 시 다음 스테이지로 진행
- Run 실패 시 처음부터 재시작

### 3.4 배터리 시스템
- 드론의 배터리가 지속적으로 소모됨
- 조명 사용 시 추가 배터리 소모
- 배터리 방전 시 게임 오버
- PowerRoom에서 배터리 충전 가능

### 3.5 Signal 수집 시스템
- 시설 내 아이템에서 Signal 추출
- 각 아이템마다 Signal 수량 범위가 다름
- 추출 시간과 소음 레벨이 아이템마다 상이
- Signal Grade에 따라 희귀도 구분 (0~3)

---

## 4. 콘텐츠 구성 (Content)

### 4.1 스테이지
- 프로시저럴 생성 기반 시설 던전
- 그리드 기반 방 배치 시스템
- Seed 기반으로 동일한 맵 재생성 가능
- 각 스테이지마다 목표 Signal 수량 설정

### 4.2 방 타입 (Room Types)
| 유형 | 특징 |
|----|----|
| Start | 플레이어 시작 지점 |
| Corridor | 연결 통로 |
| Objective | 목표 지점 (Exit 위치) |
| Storage | 아이템 파밍 구역 |
| PowerRoom | 배터리 충전 구역 |
| Lab | 특수 아이템 구역 |

### 4.3 적 캐릭터
- 기본 EnemyBase 클래스 기반
- 상태 머신: Idle → Combat → Dead
- AI 컴포넌트 기반 행동 패턴
- 플레이어의 소음에 반응

### 4.4 아이템
- SignalItemActor: Signal을 추출할 수 있는 아이템
- 아이템 아키타입 시스템으로 다양한 종류 지원
- Signal 수량 범위, 추출 시간, 소음 레벨, 등급 설정
- 스캔 시스템으로 아이템 탐지 가능

### 4.5 능력 시스템 (GAS)
- Unreal Engine의 Gameplay Ability System 사용
- 드론 능력:
  - LightToggle: 조명 토글
  - Scan: 주변 아이템 스캔
  - Extract: Signal 추출
  - PassiveBattery: 배터리 자동 소모
  - Exit: 탈출 능력

---

## 5. UI / UX

### 5.1 HUD 구성
- 배터리 상태 표시
- Signal 수집량 표시
- 스캔 하이라이트 시스템
- Run 정보 표시 (현재 스테이지, 누적 Signal 등)

### 5.2 메뉴
- 메인 메뉴
- 인게임 일시정지 메뉴
- Run 결과 화면

---

## 6. 아트 & 사운드 방향성

### 6.1 아트 스타일
- 3D 리얼리즘 스타일
- 미래적 SF 시설 환경
- 어두운 분위기와 조명 연출

### 6.2 사운드
- 드론 이동 효과음
- Signal 추출 효과음
- 배터리 경고음
- 환경음 및 분위기 음악

---

## 7. 기술 사양 (Technical)

### 7.1 개발 엔진
- Unreal Engine 5
- C++ 및 Blueprint 하이브리드 개발

### 7.2 핵심 시스템
- Gameplay Ability System (GAS)
- 프로시저럴 던전 생성 시스템
- Run 기반 서브시스템
- Attribute System (배터리, 이동속도, 스캔 범위)

### 7.3 해상도
- 기본: 1920 x 1080
- 다양한 해상도 지원

---

## 7.4 스캔 파동 VFX (공간 확산)
- 목표: 스캔 시 공간에 실제 파동이 퍼져나가는 느낌 연출
- 구성: Niagara + 머티리얼 + (선택) 후처리

### 7.4.1 Niagara 세팅
- 타입: Niagara System (Burst 1회)
- 렌더러: Mesh 또는 Sprite (Sphere/Disc)
- 파라미터:
  - `ScanCenter` (Vector, 월드 좌표)
  - `ScanRadius` (Float, 시간 경과로 증가)
  - `ScanWidth` (Float, 링 두께)
  - `ScanColor` (LinearColor)
  - `ScanSpeed` (Float, 확산 속도)
- 업데이트:
  - `ScanRadius = ScanSpeed * Age`
  - 시스템 스케일 또는 사용자 파라미터로 머티리얼에 전달

### 7.4.2 머티리얼 로직
- 월드 좌표 기반 링 계산:
  - `Distance = length(WorldPos - ScanCenter)`
  - `Ring = smoothstep(Radius - Width, Radius, Distance) - smoothstep(Radius, Radius + Width, Distance)`
- 시각 강화:
  - 링 경계에 노이즈 스크롤 추가
  - Emissive 강조 + Bloom 유도

### 7.4.3 선택 사항
- 파동 뒤 잔상: 링 반지름을 약간 늦게 따라오는 보조 링 1~2개 추가
- 환경 반응: 스캔 시 근처 오브젝트에 짧은 하이라이트 (Material Parameter Collection 연동)

### 7.4.4 구현 메모
- 스캔 능력 발동 시 `ScanCenter`와 시작 시간 전달
- Niagara System은 월드에 스폰 후 자동 소멸 (파동 끝나면 Auto Destroy)

---

## 8. 게임 플로우

### 8.1 Run 시작
1. RunSubsystem에서 새 Run 시작
2. RunSeed 생성
3. 첫 번째 스테이지 생성

### 8.2 스테이지 플레이
1. FacilityGenerator가 프로시저럴 맵 생성
2. 플레이어가 드론으로 탐험
3. Signal 아이템 수집
4. 목표 Signal 수량 달성
5. Exit 지점으로 탈출

### 8.3 스테이지 클리어
1. 수집한 Signal이 Run에 누적
2. 다음 스테이지로 진행
3. 모든 스테이지 클리어 시 Run 완료

### 8.4 게임 오버 조건
- 배터리 방전
- 탈출 실패
- Run 실패 시 처음부터 재시작

---

## 9. 상세 요약

### 9.1 목표와 금지사항
- 목표: 드론 탐험/Signal 수집/탈출 루프가 명확히 동작하도록 구현
- 금지: 직접 전투 중심의 시스템 추가 (현재는 회피/스텔스 중심)
- 금지: Run 진행 규칙(스테이지 진행/실패 처리) 변경 시 명확한 근거 없이 수정

### 9.2 핵심 게임 루프 (요약)
1. Run 시작 → Stage 생성(Seed 기반)
2. 드론 탐험 → 아이템 스캔/추출 → Signal 수집
3. 목표 Signal 달성 → Exit 탈출
4. 다음 스테이지 진행 또는 Run 종료

### 9.3 주요 시스템과 책임
- Run 관리: `USignalRunSubsystem` (Run 시작/스테이지 진행/실패 처리)
- 스테이지 생성: `ASignalFacilityGenerator` (그리드/Room 배치/스폰)
- 스테이지 상태: `ASignalGameState` (Signal 누적/클리어 조건)
- 아이템/Signal: `ASignalItemActor` + `FSignalItemArchetype`
- 드론 능력(GAS): `UDroneCoreAttributeSet` + 능력 클래스들

### 9.4 구현 시 우선순위
- P0: 스테이지 생성 → 플레이어 스폰 → 기본 이동/스캔/추출/탈출
- P1: 배터리 소모/충전/경고 연출
- P2: 적 AI 반응(소음/탐지)과 경로/패턴 개선
- P3: UI/UX(아이콘/미니맵/경고) 강화

### 9.5 데이터/에셋 의존
- Stage 설정: `USignalStageConfig` (TargetSignalSupply 등)
- 아이템 테이블: `USignalItemSet` / `FSignalItemArchetype`
- Room 세트: `USignalRoomSet` + Room BP

### 9.6 테스트 체크리스트
- StageSeed 고정 시 동일 맵 재생성 여부
- Signal 추출량/시간/소음이 아키타입 설정과 일치 여부
- 목표 Signal 달성 후 Exit로만 클리어되는지 여부
- Run 진행/실패/재시작 루프 정상 동작 여부

### 9.7 컨텐츠 확장 계획 (Run 종료 강화)
- 강화 트리거: Run 종료 시 자동 강화 (플레이어 선택 없음)
- 보너스 산정: `Bonus = max(0, FinalSignal - TargetSignal)`
- 강화 방식: 가중치 랜덤 풀에서 자동 선택 (인간에 의한 외부 강화 연출)
- 강화 대상 예시: 이동속도, 추출 속도, 배터리 최대치, 스캔 범위, 배터리 소모 감소
- 분배 규칙: 초과 Signal 구간별 강화 횟수 또는 보너스 포인트 기반 랜덤 분배
- 밸런싱: 스탯별 최대 중첩 제한, 희귀 스탯 가중치 조정, 상한(cap) 설정


