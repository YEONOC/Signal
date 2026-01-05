// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "SignalRunInfoDisplayComponent.generated.h"

class USignalRunInfoWidget;
class ASignalRoomBase;

/**
 * Start Room에 추가할 Run 정보 표시 컴포넌트
 * 플레이어가 근처에 오면 UI를 표시/숨김
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIGNAL_API USignalRunInfoDisplayComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	USignalRunInfoDisplayComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	// Overlap 이벤트
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// UI 표시/숨김
	void ShowRunInfo();
	void HideRunInfo();
	void UpdateRunInfo();

public:
	// UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USignalRunInfoWidget> RunInfoWidgetClass;

	// 상호작용 가능한 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	float InteractionRadius = 500.0f;

private:
	UPROPERTY()
	TObjectPtr<USignalRunInfoWidget> RunInfoWidget;

	UPROPERTY()
	TWeakObjectPtr<class APlayerController> OverlappedPlayerController;

	// 업데이트 타이머
	FTimerHandle UpdateTimerHandle;
};

