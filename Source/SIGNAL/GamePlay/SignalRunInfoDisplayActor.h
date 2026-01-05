// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "SignalRunInfoDisplayActor.generated.h"

class USignalRunInfoWidget;

/**
 * Start Room에 배치할 Run 정보 디스플레이 액터
 * 3D 공간에 UI를 렌더링하는 WidgetComponent를 사용
 */
UCLASS()
class SIGNAL_API ASignalRunInfoDisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASignalRunInfoDisplayActor(const FObjectInitializer& ObjectInitializer);

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
	float InteractionRadius = 1000.0f;

	// 디스플레이 크기 (WidgetComponent의 Draw Size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	FIntPoint DisplaySize = FIntPoint(1920, 1080);

	// 항상 표시할지 여부 (true면 Overlap 없이도 항상 표시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	bool bAlwaysVisible = false;

protected:
	// Widget Component - 3D 공간에 UI 렌더링
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	// Trigger Sphere - 플레이어 감지용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

private:
	UPROPERTY()
	TObjectPtr<USignalRunInfoWidget> RunInfoWidget;

	UPROPERTY()
	TWeakObjectPtr<class APlayerController> OverlappedPlayerController;

	// 업데이트 타이머
	FTimerHandle UpdateTimerHandle;
};


