// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerBase.h"
#include "BattleRoyalePlayerController.generated.h"

class ABattleRoyalePlayerState;

UCLASS()
class EXILESOFETERNITY_API ABattleRoyalePlayerController : public APlayerControllerBase
{
	GENERATED_BODY ()
	
public:
	virtual void Tick (float DeltaTime) override;

	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void SelectStartingZone (int zoneIndex);

	void AutoSelectStartingZone ();

	UFUNCTION (BlueprintCallable)
	bool GetStartingZoneChosen ();
	UFUNCTION (BlueprintCallable)
	int GetChosenStartingZone ();

	void RegisterGameStart ();
	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStart ();

	void ActivateRedeemTimer (bool state);

protected:
	virtual void BeginPlay () override;

private:
	UPROPERTY (Replicated)
	int _chosenStartingZone = 0;

	bool _gameStarted = false;

	bool _redeemTimerActivated = false;
	ABattleRoyalePlayerState* _playerState;
};
