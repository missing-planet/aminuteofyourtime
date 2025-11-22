// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "CardGamePawnBase.h"

// Sets default values
ACardGamePawnBase::ACardGamePawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACardGamePawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACardGamePawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACardGamePawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

