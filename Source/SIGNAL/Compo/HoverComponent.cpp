#include "HoverComponent.h"
#include "GameFramework/Actor.h"

UHoverComponent::UHoverComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHoverComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        BaseLocation = Owner->GetActorLocation();

        if (bUseRandomPhase)
        {
            Phase = FMath::FRand() * PI * 2.f;
        }
        bInitialized = true;
    }
}

void UHoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bInitialized) return;

    const float Time = GetWorld()->GetTimeSeconds();

    float OffsetZ = FMath::Sin(Time * Speed + Phase) * Amplitude;

    float OffsetX = 0.f;
    float OffsetY = 0.f;

    if (bAffectXY)
    {
        OffsetX = FMath::Sin(Time * (Speed * 1.3f) + Phase) * XYAmplitude;
        OffsetY = FMath::Cos(Time * (Speed * 1.7f) + Phase) * XYAmplitude;
    }

    if (AActor* Owner = GetOwner())
    {
        FVector NewLoc = BaseLocation;
        NewLoc.Z += OffsetZ;
        NewLoc.X += OffsetX;
        NewLoc.Y += OffsetY;

        Owner->SetActorLocation(NewLoc);
    }
}
