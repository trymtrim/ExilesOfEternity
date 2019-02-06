// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleRoyaleGameState.generated.h"

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	void SetStartingZoneTaken (int zoneIndex);

	UFUNCTION (BlueprintCallable)
	int GetGameStartTime ();
	UFUNCTION (BlueprintCallable)
	bool GetStartingZoneAvailable (int zoneIndex);
	UFUNCTION (BlueprintCallable)
	bool GetGameStarted ();

	UFUNCTION (BlueprintCallable)
	int GetStage ();

protected:
	virtual void BeginPlay () override;

private:
	void StartGame ();

	int _gameStartTime = 5.0f;

	UPROPERTY (Replicated)
	TArray <int> _takenStartingZones;

	UPROPERTY (Replicated)
	bool _gameStarted = false;

	int _stage = 0;
};
