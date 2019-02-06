// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerBase.h"
#include "BattleRoyalePlayerController.generated.h"

UCLASS()
class EXILESOFETERNITY_API ABattleRoyalePlayerController : public APlayerControllerBase
{
	GENERATED_BODY ()
	
public:
	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void SelectStartingZone (int zoneIndex);

	UFUNCTION (BlueprintCallable)
	bool GetStartingZoneChosen ();
	UFUNCTION (BlueprintCallable)
	int GetChosenStartingZone ();

	void RegisterGameStart ();
	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStart ();

protected:
	virtual void BeginPlay () override;
	virtual void Tick (float DeltaTime) override;

private:
	UPROPERTY (Replicated)
	int _chosenStartingZone = 0;

	bool _gameStarted = false;
};
