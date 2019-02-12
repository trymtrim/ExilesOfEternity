// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "BattleRoyaleGameState.generated.h"

class APlayAreaCircle;
class ABattleRoyalePlayerState;

UCLASS(BlueprintType, Blueprintable)
class EXILESOFETERNITY_API UGameStageInfo : public UDataAsset
{
	GENERATED_BODY ()

public:
	UPROPERTY (EditAnywhere)
	TArray <FVector> StageEndPositions;
	UPROPERTY (EditAnywhere)
	TArray <int> StageDurations;
	UPROPERTY (EditAnywhere)
	TArray <int> StageDiameters;
	UPROPERTY (EditAnywhere)
	float ShrinkSpeed;
	UPROPERTY (EditAnywhere)
	int GameStartTime;
	UPROPERTY (EditAnywhere)
	int AmountOfSpellCapsulesForEachSpell;
	UPROPERTY (EditAnywhere)
	float RedeemKillTime;
};

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	void LoadGameStageInfo (UGameStageInfo* gameStageInfo);

	void SetStartingZoneTaken (int zoneIndex);
	void StartGame ();
	void StartNextStage ();
	void ReportPermanentDeath (ABattleRoyalePlayerState* playerState);

	UFUNCTION (BlueprintCallable)
	int GetGameStartTime ();
	UFUNCTION (BlueprintCallable)
	bool GetStartingZoneAvailable (int zoneIndex);
	UFUNCTION (BlueprintCallable)
	bool GetGameStarted ();
	UFUNCTION (BlueprintCallable)
	float GetStartTime ();

	UFUNCTION (BlueprintCallable)
	int GetStage ();
	UFUNCTION (BlueprintCallable)
	int GetStageDuration (int stageIndex);
	UFUNCTION (BlueprintCallable)
	FVector GetCircleLocation ();
	UFUNCTION (BlueprintCallable)
	FVector GetCircleScale ();

	UFUNCTION (BlueprintCallable)
	float GetRedeemKillTime ();

	UFUNCTION (BlueprintCallable)
	bool GetSpawnPositionInsidePlayArea (int spawnIndex);

protected:
	virtual void BeginPlay () override;

private:
	void EndGame ();

	UPROPERTY (Replicated)
	TArray <int> _takenStartingZones;

	UPROPERTY (Replicated)
	bool _gameStarted = false;

	UPROPERTY (Replicated)
	int _stage = 1;

	UGameStageInfo* _gameStageInfo;
	APlayAreaCircle* _playAreaCircle;

	TArray <ABattleRoyalePlayerState*> _permanentDeadPlayers;

	UPROPERTY (Replicated)
	float _startTime;
	bool _gameEnded = false;
};
