# GAS (Gameplay Ability System)

## 설정 위치

`ADroneCharacter`가 `IAbilitySystemInterface`를 구현.
- ASC: `USignalAbilitySystemComponent`
- AttributeSet: `UDroneCoreAttributeSet`
- Ability 부여: `ADroneCharacter::GiveStartupAbilities()`

---

## Attributes (`UDroneCoreAttributeSet`)

| Attribute | 설명 |
|---|---|
| `Battery` | 현재 배터리 잔량 |
| `BatteryMax` | 배터리 최대치 |
| `MoveSpeed` | 드론 이동 속도 |
| `ScanRange` | 스캔 탐지 반경 |

수치는 `USignalStageConfig` 데이터 에셋으로 관리. 코드에 하드코딩 금지.

---

## Abilities

| Ability 클래스 | 기능 |
|---|---|
| `GA_Drone_LightToggle` | 드론 스포트라이트 토글 |
| `GA_Drone_Scan` | 구체 스캔, `GCue_Drone_ScanPulse` Niagara VFX 트리거 |
| `GA_Drone_Extract` | Signal 추출 인터랙션 |
| `GA_Drone_PassiveBattery` | 배터리 자동 소모 패시브 틱 |
| `GA_Drone_Exit` | Exit 지점 탈출 인터랙션 |

---

## Gameplay Tags

모든 태그는 `FSignalGameplayTags` (`SignalGameplayTags.h`)에 집중 관리.

**새 태그 추가 시 반드시:**
1. `SignalGameplayTags.h`에 멤버 변수 선언
2. `FSignalGameplayTags::InitializeNativeTags()`에 `ADD_NATIVE_TAG` 등록
3. 태그를 직접 문자열로 사용하지 말 것

---

## 새 Ability 추가 절차

1. `SignalGameplayTags.h`에 관련 Tag 추가 → `InitializeNativeTags()` 등록
2. `UDroneCoreAttributeSet`에 신규 Attribute 필요 여부 판단
3. `GA_Drone_*` 클래스 생성 (`Source/SIGNAL/AbilitySystem/Abilities/`)
4. `ADroneCharacter::GiveStartupAbilities()`에 Grant 추가
5. 필요 시 GameplayCue (`GCue_*`) 추가 및 Niagara 연동

---

## Scan VFX 구조 (GA_Drone_Scan)

Niagara System (`GCue_Drone_ScanPulse`) 파라미터:

| 파라미터 | 타입 | 용도 |
|---|---|---|
| `ScanCenter` | Vector | 스캔 발생 월드 좌표 |
| `ScanRadius` | Float | 시간 경과로 증가 (`ScanSpeed * Age`) |
| `ScanWidth` | Float | 링 두께 |
| `ScanColor` | LinearColor | 링 색상 |
| `ScanSpeed` | Float | 확산 속도 |

머티리얼 링 계산: `Distance = length(WorldPos - ScanCenter)` → smoothstep으로 링 마스킹.
스캔 능력 발동 시 `ScanCenter`와 시작 시간 전달 후 Auto Destroy.
