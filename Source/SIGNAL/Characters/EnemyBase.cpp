 // Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    /*Root = CreateDefaultSubobject<USceneComponent>("Root");
    SetRootComponent(Root);*/

    // BodyCollision = CreateDefaultSubobject<UCapsuleComponent>("Collision");

    /*Health = CreateDefaultSubobject<UHealthComponent>("Health");
    Targeting = CreateDefaultSubobject<UTargetingComponent>("Targeting");
    Weapon = CreateDefaultSubobject<UWeaponComponent>("Weapon");*/

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //switch (State)
    //{
    //case EEnemyState::Idle:   HandleIdle(DeltaTime); break;
    //case EEnemyState::Combat: HandleCombat(DeltaTime); break;
    //case EEnemyState::Dead:   /* no-op */ break;
    //}

}

void AEnemyBase::HandleIdle(float DeltaTime)
{
    /*if (Targeting && Targeting->HasValidTarget())
    {
        SetState(EEnemyState::Combat);
    }*/
}

void AEnemyBase::HandleCombat(float DeltaTime)
{
}

void AEnemyBase::OnDeath()
{
}

void AEnemyBase::SetState(EEnemyState NewState)
{
    State = NewState;
}

