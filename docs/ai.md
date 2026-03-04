# AI System

## 상태 머신 (AEnemyBase)

```
Idle → Combat → Dead
```

모든 적은 `AEnemyBase`를 상속. 상태 전환은 BehaviorTree + Blackboard로 구동.

---

## AEnemySentryDrone

`AEnemyBase` 상속. 현재 구현된 유일한 구체 적 클래스.

- **감지 방식:** 콘 라인 트레이스 (`CanSeeTarget()`)
- **페널티:** 플레이어 배터리 Drain 증가
- **HUD 반응:** 감지 상태에 따라 윤곽 색상 변화

---

## BehaviorTree 서비스

| 서비스 클래스 | 역할 |
|---|---|
| `UBTService_SentrySense` | Blackboard의 감지 키 업데이트 |
| `UBTService_SentryCombat` | 전투 상태 전환 처리 |

### Blackboard 키

| 키 | 용도 |
|---|---|
| `TargetActorKey` | 감지된 플레이어 액터 참조 |
| `LastKnownLocationKey` | 마지막으로 확인된 플레이어 위치 |
| `HasLineOfSightKey` | 현재 직선 시야 확보 여부 |

---

## 계획된 적 타입 (MVP 우선순위)

### 1순위: 센트리 드론 (구현 완료)
- 역할: 탐험/추출 중 긴장감 형성
- 행동: Corridor 순찰 → 시야 감지 → 추적
- 구현: AIController + MoveTo + 라인트레이스

### 2순위: 스태틱 터렛 (미구현)
- 역할: 특정 구역 접근 리스크 부여
- 행동: 고정 설치, 각도/범위 스윕 → 경고 → 발사
- 페널티: 배터리 급감 또는 라이트 강제 오프
- 구현: 라인트레이스/각도 계산 기반, NavMesh 불필요
- HazardLevel: Dangerous ~ Critical

---

## 새 적 추가 절차

1. `AEnemyBase` 상속 클래스 생성
2. BehaviorTree / Blackboard 에셋 생성
3. `AEnemyAIController` 연결 또는 전용 AIController 파생
4. `UBTService_*` 서비스 작성 (Sense / Combat 분리)
5. `USignalRoomSet` 또는 스폰 규칙에 적 타입 등록
6. HazardLevel 설정 및 HUD 연동 확인
