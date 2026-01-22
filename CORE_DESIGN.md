# CORE_DESIGN.md

## Game Core Design Document (Single Source of Truth)

---

## 1. 게임 개요

### 게임명

SIGNAL

### 장르

* 3D 액션 탐험 / Roguelite
* 싱글 플레이

### 핵심 플레이 경험 (Core Fantasy)

플레이어는 전투 능력보다 **탐험, 리스크 관리, 탈출 판단**을 통해
Signal을 수집하고 생존한다.

이 게임에서 중요한 것은 “얼마나 잘 싸웠는가”가 아니라
“언제 추출하고 언제 탈출했는가”이다.

---

## 2. 전체 게임 루프 (Global Game Loop)

```
[Run 시작]
   ↓
[Stage 생성(Seed 기반)]
   ↓
[탐험/스캔/추출]
   ├─ Signal 수집
   ├─ 배터리 소모
   └─ 적 반응(소음)
   ↓
[Exit 탈출]
   ↓
[Run 진행/종료]
   ├─ 다음 Stage 시작
   └─ Run 종료 후 강화/결과
   ↓
(반복)
```

모든 시스템은 반드시 이 루프 중 **어느 단계에 속하는지 명확해야 한다.**

---

## 3. 설계 철학 (Design Principles)

### 3.1 위험-보상 명확성

* 추출 시간, 배터리 소모는 **명확한 비용**이다.
* 고등급 Signal일수록 리스크(추출 시간 등)가 커야 한다.

### 3.2 정보 가시성

* Signal 수치, 배터리, 스캔 결과는 **UI로 즉시 피드백**된다.
* 숨겨진 규칙은 최소화하고, 필요 시 연출로 명확히 전달한다.

### 3.3 재현 가능성

* 동일 Seed → 동일 Stage 생성
* Run 진행과 보상은 **Seed 및 규칙 기반**으로 재현 가능해야 한다.

---

## 4. 주요 시스템 구성

### 4.1 Run/Stage System

* Run은 여러 Stage로 구성된다.
* Stage는 프로시저럴 생성되며 Seed로 고정된다.

책임:

* Run 시작/종료 처리
* Stage 진행 및 실패 관리
* Signal 누적 및 목표 달성 판정

---

### 4.2 Exploration/Interaction System

* 드론 이동, 스캔, 추출, 탈출이 핵심 행동이다.
* 전투는 직접 조작 중심이 아니라 **회피/스텔스 중심**이다.

책임:

* 이동/시야 조작
* 스캔을 통한 아이템 탐지
* 추출 상호작용 및 Signal 획득
* Exit 탈출 처리

---

### 4.3 Ability/Attribute System (GAS)

* 드론은 Ability와 Attribute로 정의된다.
* Ability는 라이트, 스캔, 추출, 배터리 소모, 탈출 등이다.

책임:

* 배터리, 이동속도, 스캔 범위 관리
* 능력 발동 및 쿨다운/상태 처리

---

### 4.4 Progression System (Run 종료 강화)

* Run 종료 시 초과 Signal에 따라 **랜덤 강화**가 적용된다.
* 플레이어 선택 없음, 외부(인간) 강화 연출.

책임:

* `Bonus = max(0, FinalSignal - TargetSignal)` 계산
* 가중치 랜덤 풀로 능력치 강화
* 상한/중첩 제한으로 밸런스 유지

#### 4.4.1 추천 수치 (초안)

* 보너스 포인트: `BonusPoint = floor(Bonus / 25)` (초과 25마다 1포인트)
* 강화 횟수: `BonusPoint`만큼 랜덤 강화 적용
* 중첩 제한: 동일 스탯 최대 3회

강화 풀 및 가중치:

| 스탯 | 1회 강화량 | 가중치 |
|---|---:|---:|
| 이동속도 | +4% | 30 |
| 추출 속도 | +6% | 25 |
| 배터리 최대치 | +8 | 20 |
| 스캔 범위 | +6% | 15 |
| 배터리 소모 감소 | -4% | 10 |

상한(cap) 예시:

* 이동속도: +12% (3회)
* 추출 속도: +18% (3회)
* 배터리 최대치: +24 (3회)
* 스캔 범위: +18% (3회)
* 배터리 소모 감소: -12% (3회)

---

### 4.5 Enemy AI (MVP: 1~2순위)

기본 루프 완성을 위해 아래 2종을 먼저 구현한다.

#### 4.5.1 센트리 드론 (Patrol + Spotlight + Chase)

* 역할: 탐험/추출 중 긴장감 즉시 형성
* 행동: Corridor 순찰 → 시야 감지 → 일정 시간 추적
* 페널티: 배터리 Drain 증가 또는 Extract/Exit 취소
* 구현: AIController + MoveTo + Perception(또는 라인트레이스)
* Scan/HUD: 감지 상태에 따라 윤곽 색상 변화 (HazardLevel: Medium~Dangerous)

#### 4.5.2 스태틱 터렛 (라인 오브 사이트 + 지역 통제)

* 역할: 특정 구역 접근 리스크 부여, 방 가치와 위험 균형
* 행동: 고정 설치, 각도/범위 스윕 → 경고 → 발사
* 페널티: 배터리 급감 또는 라이트 강제 오프
* 구현: 라인트레이스/각도 계산 기반, Nav 불필요
* Scan/HUD: 위험 구역 즉시 인지 (HazardLevel: Dangerous~Critical)

---

### 4.6 아이템/적 스폰 구현 사항

#### 4.6.1 스폰 파이프라인

* Stage 생성 시 `ASignalFacilityGenerator`가 Room을 스폰
* Room의 `ItemSpawnPoints` / `EnemySpawnPoints`를 수집
* 수집된 포인트에 대해 **Seed 기반 랜덤 분배**로 스폰
* 스폰은 Stage 시작 시 1회만 수행 (런타임 리스폰 없음)

#### 4.6.2 아이템 스폰 규칙

* 아이템 풀은 `USignalItemSet` / `FSignalItemArchetype` 기반
* `PreferredRooms`와 `SpawnWeight`를 활용해 가중치 선택
* Start Room에는 스폰하지 않음 (초기 안전 확보)
* 추출 후 아이템은 파괴되어 중복 추출 불가

#### 4.6.3 적 스폰 규칙

* 적 스폰은 Room 타입과 ThreatLevel을 기준으로 난이도 스케일
* Corridor/Objective에 위험도를 높게 배치 (동선 압박)
* 스폰 시 NavMesh 유효성 확인 및 시야 차단물 고려
* MVP 단계에서는 동적 증원/웨이브 없음

#### 4.6.4 동기화/표시

* Scan 결과와 스폰 객체는 `HazardLevel`/Signal 등급으로 표시
* HUD는 **스폰 상태를 직접 소유하지 않으며** 이벤트만 수신

---

## 5. 데이터 구조 원칙

* Stage/Item/Room/Ability 수치는 **데이터 에셋 기반**으로 관리한다.
* 로직 변경 없이 수치 조정 가능해야 한다.

예:

* `USignalStageConfig`
* `USignalItemSet` / `FSignalItemArchetype`
* `USignalRoomSet`

---

## 6. 절대 규칙 (Hard Rules)

아래 규칙은 어떤 상황에서도 위반되어서는 안 된다.

1. UI는 절대 게임 로직을 소유하지 않는다.
2. Stage 생성/Run 결과는 Seed 및 규칙 기반으로 재현 가능해야 한다.
3. 테스트용 코드가 릴리즈 구조에 남지 않는다.
4. 시스템 간 직접 참조는 최소화하고, 이벤트/인터페이스를 우선한다.
5. 전투 중심 게임으로 변질시키지 않는다.

---

## 7. 확장 방향 (Future Considerations)

* 멀티플레이는 현재 고려하지 않는다.
* Stage/Room/Item 데이터 확장으로 콘텐츠를 확장한다.
* Run 종료 강화 풀을 지속적으로 추가/밸런싱한다.
* Scan VFX/환경 반응을 고도화하여 탐험 피드백을 강화한다.

---

## 8. Agent 사용 지침

Cursor Agent는 작업 시 다음을 기준으로 판단한다.

* 이 기능은 게임 루프의 어느 단계에 속하는가?
* 기존 시스템 중 책임이 겹치는 것은 없는가?
* CORE_DESIGN.md의 Hard Rules를 위반하지 않는가?

Agent는 이 문서를 **최상위 규칙 문서로 간주한다.**

---

## 9. 문서 관리 원칙

* 이 문서는 항상 최신 상태를 유지한다.
* 설계 변경 시 코드보다 먼저 수정한다.
* 이 문서와 충돌하는 코드는 잘못된 코드로 간주한다.

