// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScanHighlightComponent.generated.h"

class UMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIGNAL_API UScanHighlightComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UScanHighlightComponent();

    UFUNCTION(BlueprintCallable, Category = "Scan")
    void HighlightForScan(float Duration);

protected:
    virtual void BeginPlay() override;

private:
    void SetHighlighted(bool bEnabled);
    void ClearHighlight();

public:
    UPROPERTY(EditAnywhere, Category = "Scan")
    int32 HazardLevel = 0; // 0~3 정도

protected:
    // 하이라이트에 쓸 Stencil 값 (머티리얼에서 이 값으로 필터링할 수도 있음)
    UPROPERTY(EditAnywhere, Category = "Scan")
    int32 StencilValue = 1;


private:
    UPROPERTY()
    TArray<UMeshComponent*> MeshComponents;

    FTimerHandle HighlightTimerHandle;

    
};
