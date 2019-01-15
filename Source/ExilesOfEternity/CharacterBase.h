// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class EXILESOFETERNITY_API ACharacterBase : public ACharacter
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	ACharacterBase ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent (class UInputComponent* PlayerInputComponent) override;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;
};
