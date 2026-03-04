# CLAUDE.md

SIGNAL — 3D 액션 탐험 / Roguelite (UE5, C++ + Blueprint 하이브리드)
드론으로 프로시저럴 시설을 탐험, Signal 수집 후 탈출. 전투보다 스텔스/리스크 관리가 핵심.

## Build

- **Rider:** `SIGNAL.uproject` 우클릭 → Build
- **CLI:** `UnrealBuildTool SIGNALEditor Win64 Development`
- **프로젝트 파일 재생성:** `SIGNAL.uproject` 우클릭 → Generate Visual Studio project files
- 자동화 테스트 없음. 수동 검증은 `docs/gameplay.md` 테스트 체크리스트 참조

## 절대 규칙 (Hard Rules)

1. **UI는 게임 로직을 소유하지 않는다** — 델리게이트/이벤트 수신만
2. **Stage 생성/Run 결과는 Seed 재현 가능**해야 한다
3. **테스트/디버그 코드를 프로덕션 경로에 남기지 않는다**
4. **시스템 간 직접 참조 최소화** — 델리게이트와 인터페이스 우선
5. **직접 전투 중심으로 변질 금지** — 회피/스텔스가 핵심

## 모듈 의존성 (`SIGNAL.Build.cs`)

`GameplayAbilities`, `GameplayTags`, `GameplayTasks`, `AIModule`, `EnhancedInput`, `UMG`, `StateTreeModule`

Include 경로: `#include "Characters/EnemyBase.h"` (SIGNAL 루트가 PublicIncludePaths에 등록됨)

## 참조 문서 (`docs/`)

| 파일 | 내용 |
|---|---|
| `docs/architecture.md` | 클래스 책임 테이블, 프로시저럴 생성 파이프라인, 모듈 의존성 |
| `docs/gameplay.md` | 게임 루프, Room 타입, Signal/배터리 시스템, 테스트 체크리스트 |
| `docs/gas.md` | Attribute, Ability, Gameplay Tag 구조 및 추가 절차 |
| `docs/ai.md` | BT/BB 구조, 적 타입, 신규 적 추가 절차 |
| `docs/data-assets.md` | Config 에셋 목록, FSignalItemArchetype 필드 |
| `docs/progression.md` | Run 강화 시스템, 수치 공식, 강화 풀 |
