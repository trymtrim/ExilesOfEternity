// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerBase.h"
#include "BattleRoyalePlayerController.generated.h"

class ABattleRoyalePlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams (FOnLevelUpMessage, FString, message, int, level);

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
	UFUNCTION (BlueprintCallable)
	bool GetGameStarting ();
	UFUNCTION (BlueprintCallable)
	int GetGameStartingWorldSeconds ();

	void RegisterGameStarting ();
	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStarting (int currentWorldSeconds);

	void RegisterGameStart ();
	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStart ();

	void ActivateRedeemTimer (bool state);

	UFUNCTION (Client, Reliable)
	void ShowLevelUpMessage (const FString& message, int level);

	UPROPERTY (BlueprintAssignable)
	FOnLevelUpMessage OnLevelUpMessageBP;

	UFUNCTION (BlueprintCallable)
	void StartSpectating ();
	void ChangeSpectatingTarget ();

	UPROPERTY (BlueprintReadOnly)
	bool _gameStarted = false;

protected:
	virtual void BeginPlay () override;

private:
	void ResetPlayerCharacter (FVector spawnLocation);

	UPROPERTY (Replicated)
	int _chosenStartingZone = 0;

	bool _gameStarting = false;
	int _gameStartingWorldSeconds;

	bool _redeemTimerActivated = false;
	ABattleRoyalePlayerState* _playerState;

	UPROPERTY (Replicated)
	int _recentPlayerSpawnPosition;
	int _selectedPlayerSpawnPosition = 0;

	int _spectatingIndex = 0;
};
