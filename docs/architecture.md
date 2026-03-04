# Architecture

## Core Game Loop

```
[Run 시작]
   ↓
[Stage 생성 (Seed 기반)] ← ASignalFacilityGenerator
   ↓
[탐험 / 스캔 / 추출]     ← ADroneCharacter + GAS
   ↓
[Exit 탈출]
   ↓
[Run 진행 / 종료]        ← USignalRunSubsystem
   ↓
(반복)
```

모든 신규 시스템은 반드시 위 루프의 **어느 단계에 속하는지** 명확해야 한다.

---

## System Responsibilities

| Class | Responsibility |
|---|---|
| `ASIGNALGameMode` | Stage 흐름 조율; FacilityGenerator 이벤트를 RunSubsystem에 바인딩 |
| `USignalRunSubsystem` | GameInstanceSubsystem; Run 상태 소유 (seed, stage index, total signal, cleared/failed) |
| `ASignalFacilityGenerator` | 그리드 기반 프로시저럴 Stage 생성; Seed로 Room BP, 아이템, 적 스폰 |
| `ASignalGameState` | Stage별 Signal 누적 및 클리어 조건; `OnSignalAmountChanged` / `OnStageCleared` 발생 |
| `ADroneCharacter` | 플레이어 드론; `USignalAbilitySystemComponent` + `UDroneCoreAttributeSet` 소유 |
| `ASignalRoomBase` | 모든 Room BP의 기반 클래스; `ItemSpawnPoints` / `EnemySpawnPoints` 노출 |
| `ASignalItemActor` | 추출 가능한 Signal 아이템; `FSignalItemArchetype`으로 초기화 |
| `AEnemyBase` | 추상 적 기반; 상태 머신: `Idle → Combat → Dead` |
| `AEnemySentryDrone` | 구체 적 클래스; 스포트라이트 콘 시야, 플레이어 배터리 소모 |
| `AEnemyAIController` | BehaviorTree + Blackboard 기반 AIController |

---

## Procedural Generation Pipeline

`ASignalFacilityGenerator::GenerateStage(InSeed)` 호출 순서:

1. `GenerateLayout()` — 메인 경로 방 배치 (Start → Corridor → Objective)
2. `AddSideRooms()` — 사이드 셀에 Storage / PowerRoom / Lab 부착
3. `BuildDoorConnections()` — 벽별 문 연결 결정
4. `SpawnRooms()` — `USignalRoomSet`에서 Room BP 인스턴스화
5. `CollectSpawnPoints()` → `DistributeAndSpawnItems()` / `DistributeAndSpawnEnemies()` — 방 채우기
6. `OnStageGenerated` 델리게이트 — `StartTransform`과 함께 플레이어 스폰 신호

**규칙:** 동일 Seed → 동일 Stage. Seed 재현성을 깨는 변경 금지.

---

## Module Dependencies

`SIGNAL.Build.cs` 주요 모듈:

```
GameplayAbilities, GameplayTags, GameplayTasks
AIModule, EnhancedInput, UMG, StateTreeModule
```

## Include Path 규칙

`PublicIncludePaths`에 `SIGNAL` 루트가 등록되어 있으므로:

```cpp
#include "Characters/EnemyBase.h"      // O
#include "SIGNAL/Characters/EnemyBase.h" // X
```
