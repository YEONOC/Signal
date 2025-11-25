// Fill out your copyright notice in the Description page of Project Settings.


#include "ScanHighlightComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UScanHighlightComponent::UScanHighlightComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UScanHighlightComponent::BeginPlay()
{
    Super::BeginPlay();

    // 이 액터 안의 모든 MeshComponent를 모아둔다
    AActor* Owner = GetOwner();
    if (Owner)
    {
        Owner->GetComponents<UMeshComponent>(MeshComponents);
    }

    // HazardLevel에 따라 Stencil 고정
    int32 Stencil = 1; // 기본값 = 안전(초록)

    switch (HazardLevel)
    {
        case 0: Stencil = 1; break; // Green
        case 1: Stencil = 2; break; // Yellow
        case 2: Stencil = 3; break; // Red
        case 3: Stencil = 4; break; // Purple
    }
    StencilValue = Stencil;

    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (!MeshComp) continue;

        MeshComp->SetCustomDepthStencilValue(StencilValue);
        MeshComp->SetRenderCustomDepth(false); // 기본은 꺼둠, 스캔 때만 켬
    }
}

void UScanHighlightComponent::HighlightForScan(float Duration)
{
    if (Duration <= 0.f)
    {
        Duration = 1.f;
    }

    SetHighlighted(true);

    // Duration 뒤에 자동으로 종료
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(HighlightTimerHandle);
        World->GetTimerManager().SetTimer(
            HighlightTimerHandle,
            this,
            &UScanHighlightComponent::ClearHighlight,
            Duration,
            false
        );
    }
}

void UScanHighlightComponent::SetHighlighted(bool bEnabled)
{
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (!MeshComp) continue;

        MeshComp->SetRenderCustomDepth(bEnabled);

        if (bEnabled)
        {
            MeshComp->SetCustomDepthStencilValue(StencilValue);
        }
    }
}

void UScanHighlightComponent::ClearHighlight()
{
    SetHighlighted(false);
}
