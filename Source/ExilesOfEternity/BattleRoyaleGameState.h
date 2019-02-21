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

UCLASS(BlueprintType, Blueprintable)
class EXILESOFETERNITY_API UPlayerProgressionInfo : public UDataAsset
{
	GENERATED_BODY ()

public:
	UPROPERTY (EditAnywhere)
	int MaxLevel;
	UPROPERTY (EditAnywhere)
	TArray <int> ExperienceNeededPerLevel;
	UPROPERTY (EditAnywhere)
	TArray <float> HealthPerLevel;
	UPROPERTY (EditAnywhere)
	TArray <float> BasicSpellDamagePerLevel;
};

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	void LoadGameInfo (UGameStageInfo* gameStageInfo, UPlayerProgressionInfo* playerProgressionInfo);

	void SetStartingZoneTaken (int zoneIndex);
	void StartGame ();
	void StartNextStage ();
	void ReportPermanentDeath (ABattleRoyalePlayerState* playerState);

	UPlayerProgressionInfo* GetPlayerProgressionInfo ();

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

	TArray <ABattleRoyalePlayerState*> GetPermanentDeadPlayers ();
	int GetPlayerCount ();

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
	UPlayerProgressionInfo* _playerProgressionInfo;
	APlayAreaCircle* _playAreaCircle;

	int _playerCount = 0;
	TArray <ABattleRoyalePlayerState*> _permanentDeadPlayers;

	UPROPERTY (Replicated)
	float _startTime;
	bool _gameEnded = false;
};
