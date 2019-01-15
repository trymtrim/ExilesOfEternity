// Copyright Sharp Raccoon 2019.

#include "CharacterBase.h"

//Sets default values
ACharacterBase::ACharacterBase ()
{
 	//Set this character to call Tick () every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Called when the game starts or when spawned
void ACharacterBase::BeginPlay ()
{
	Super::BeginPlay ();

	GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "THIS IS A TEST YO!");
}

//Called every frame
void ACharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);
}

//Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);
}
