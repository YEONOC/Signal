// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SignalItemArchetype.h"
#include "SignalItemSet.generated.h"

UCLASS(BlueprintType)
class SIGNAL_API USignalItemSet : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FSignalItemArchetype> Items;
};
