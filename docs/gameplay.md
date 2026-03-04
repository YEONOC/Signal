# Gameplay Design

## 핵심 플레이 경험 (Core Fantasy)

플레이어는 전투 능력보다 **탐험, 리스크 관리, 탈출 판단**을 통해 Signal을 수집하고 생존한다.
중요한 것은 "얼마나 잘 싸웠는가"가 아니라 **"언제 추출하고 언제 탈출했는가"**이다.

---

## 기본 조작

| 입력 | 기능 |
|---|---|
| 이동 키 | 드론 3D 이동 (수평/수직) |
| 시야 조작 | 카메라 회전 |
| 조명 토글 | 드론 조명 켜기/끄기 |
| 스캔 | 주변 아이템 탐지 |
| 추출 | Signal 아이템 추출 |
| 탈출 | Exit 지점으로 탈출 |

---

## 배터리 시스템

- 드론 배터리는 지속적으로 소모됨 (PassiveBattery Ability)
- 조명 켤 시 추가 소모
- 적 감지 상태 시 추가 소모 (SentryDrone 페널티)
- 배터리 방전 → 게임 오버
- PowerRoom에서 충전 가능

---

## Room 타입

| 타입 | 역할 |
|---|---|
| Start | 플레이어 시작 지점. 아이템 스폰 없음 |
| Corridor | 연결 통로. 적 배치로 동선 압박 |
| Objective | Exit 위치. 메인 경로 끝 |
| Storage | 아이템 파밍 구역 |
| PowerRoom | 배터리 충전 구역 |
| Lab | 고등급 아이템 구역 |

---

## Signal 수집 시스템

- `ASignalItemActor`에서 추출 인터랙션으로 Signal 획득
- 아이템마다 Signal 수량 범위, 추출 시간, 소음 레벨, 등급(Grade 0~3) 상이
- 추출 시 발생하는 소음으로 적이 반응할 수 있음
- 추출 완료 후 아이템 파괴 → 중복 추출 불가
- 스캔(GA_Drone_Scan)으로 근처 아이템 탐지 가능

---

## 아이템 스폰 규칙

- 아이템 풀: `USignalItemSet` / `FSignalItemArchetype` 기반
- `PreferredRooms`와 `SpawnWeight`로 가중치 선택
- Start Room에는 스폰하지 않음
- Stage 시작 시 1회 스폰, 런타임 리스폰 없음

---

## 적 스폰 규칙

- Room 타입과 ThreatLevel을 기준으로 난이도 스케일
- Corridor / Objective에 위험도 높게 배치 (동선 압박)
- 스폰 시 NavMesh 유효성 확인
- MVP 단계: 동적 증원 / 웨이브 없음

---

## Stage 클리어 조건

1. Signal 목표량(`USignalStageConfig::TargetSignalSupply`) 달성
2. Exit 지점으로 탈출 (`GA_Drone_Exit`)
3. 수집 Signal이 Run에 누적 → 다음 Stage 진행

---

## 게임 오버 조건

- 배터리 방전
- 탈출 실패 (Stage 내 배터리 소진)
- Run 실패 시 처음부터 재시작

---

## 테스트 체크리스트

- [ ] StageSeed 고정 시 동일 맵 재생성 여부
- [ ] Signal 추출량/시간/소음이 아키타입 설정과 일치 여부
- [ ] 목표 Signal 달성 후 Exit로만 클리어되는지 여부
- [ ] Run 진행/실패/재시작 루프 정상 동작 여부
- [ ] Start Room에 아이템 미스폰 여부
