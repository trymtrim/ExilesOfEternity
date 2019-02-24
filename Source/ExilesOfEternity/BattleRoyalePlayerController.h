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

	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void SelectPlayerSpawnPosition (int zoneIndex);
	void SpawnPlayerCharacter (int zoneIndex);

	void AutoSpawnPlayerCharacter ();

	UFUNCTION (BlueprintCallable)
	bool GetStartingZoneChosen ();
	UFUNCTION (BlueprintCallable)
	int GetChosenStartingZone ();
	UFUNCTION (BlueprintCallable)
	int GetRecentPlayerSpawnPosition ();

	void RegisterGameStart ();
	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStart ();

	void ActivateRedeemTimer (bool state);

protected:
	virtual void BeginPlay () override;

private:
	void ResetPlayerCharacter ();

	UPROPERTY (Replicated)
	int _chosenStartingZone = 0;

	bool _gameStarted = false;

	bool _redeemTimerActivated = false;
	ABattleRoyalePlayerState* _playerState;

	UPROPERTY (Replicated)
	int _recentPlayerSpawnPosition;
	int _selectedPlayerSpawnPosition = 0;
};
