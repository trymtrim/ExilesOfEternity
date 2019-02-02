// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

UCLASS()
class EXILESOFETERNITY_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	APlayerControllerBase ();

	//Called to bind functionality to input
	virtual void SetupInputComponent () override;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual void Possess (APawn* InPawn) override;

private:
	void ShowMouseCursor (bool state);
	template <bool state>
	void ShowMouseCursor ()
	{
		ShowMouseCursor (state);
	}
};
