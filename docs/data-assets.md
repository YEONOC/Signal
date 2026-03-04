# Data Assets

## 원칙

**수치는 반드시 데이터 에셋으로 관리. 코드에 하드코딩 금지.**
로직 변경 없이 수치 조정이 가능해야 한다.

---

## 에셋 목록

### USignalStageConfig
- 용도: Stage별 설정
- 주요 필드:
  - `TargetSignalSupply` — 클리어에 필요한 Signal 목표량
  - 난이도 파라미터

### USignalRunConfig
- 용도: Run 전체 설정
- 주요 필드:
  - Stage 수
  - 시드(Seed) 목록 또는 생성 규칙

### USignalItemSet / FSignalItemArchetype
- 용도: 스폰 가능한 아이템 풀 정의
- `FSignalItemArchetype` 주요 필드:

| 필드 | 설명 |
|---|---|
| `SignalYieldRange` | Signal 획득량 범위 (min ~ max) |
| `ExtractTime` | 추출 소요 시간 (초) |
| `NoiseLevel` | 추출 시 발생 소음 수준 |
| `Grade` | 아이템 희귀도 (0 ~ 3) |
| `SpawnWeight` | 스폰 가중치 |
| `PreferredRooms` | 우선 배치 Room 타입 목록 |

### USignalRoomSet
- 용도: `ESignalRoomType` → Room Blueprint 클래스 매핑
- Stage 생성 시 `ASignalFacilityGenerator`가 참조

---

## 에셋 추가 시 주의

- 새 수치 항목은 기존 Config 클래스에 필드 추가 우선
- 새 Config 클래스가 필요할 경우 `UDataAsset` 상속
- 에셋 파일은 `Content/Data/` 하위에 저장
