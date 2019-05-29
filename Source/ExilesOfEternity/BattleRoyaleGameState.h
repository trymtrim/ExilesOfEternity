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

USTRUCT (BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY ()

	UPROPERTY (BlueprintReadOnly)
	FString Name;
	UPROPERTY (BlueprintReadOnly)
	int Kills;
	UPROPERTY (BlueprintReadOnly)
	int Deaths;

	UPROPERTY (BlueprintReadOnly)
	ABattleRoyalePlayerState* playerState;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnNewStage, int, stage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnShrinkingStopped);

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	void LoadGameInfo (UGameStageInfo* gameStageInfo, UPlayerProgressionInfo* playerProgressionInfo);

	void SetStartingZoneTaken (int zoneIndex);
	void StartGame ();
	void ReportShrinkingStopped ();
	void ReportPermanentDeath (ABattleRoyalePlayerState* playerState);
	void CheckForVictory (ABattleRoyalePlayerState* disconnectedPlayer);

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
	FVector GetNextCircleLocation ();
	UFUNCTION (BlueprintCallable)
	FVector GetNextCircleScale ();

	UFUNCTION (BlueprintCallable)
	float GetStageTimeLeft ();
	UFUNCTION (BlueprintCallable)
	float GetCircleClosingTimeLeft ();
	UFUNCTION (BlueprintCallable)
	float GetCircleClosingTimeLeftPercentage ();

	UFUNCTION (BlueprintCallable)
	float GetRedeemKillTime ();

	UFUNCTION (BlueprintCallable)
	bool GetSpawnPositionInsidePlayArea (int spawnIndex);

	TArray <ABattleRoyalePlayerState*> GetPermanentDeadPlayers ();
	int GetPlayerCount ();

	//Scoreboard
	void UpdateScoreboardPlayerStats (ABattleRoyalePlayerState* playerState);

	UFUNCTION (BlueprintCallable)
	TArray <FPlayerStats> GetAllPlayerStats ();

	UPROPERTY (BlueprintAssignable)
	FOnNewStage OnNewStageBP;
	UPROPERTY (BlueprintAssignable)
	FOnShrinkingStopped OnShrinkingStoppedBP;

protected:
	virtual void BeginPlay () override;

private:
	void StartNextStage ();

	UFUNCTION (NetMulticast, Reliable)
	void BroadcastStartNextStage (int stage);
	UFUNCTION (NetMulticast, Reliable)
	void BroadcastShrinkingStopped ();

	void EndGame ();

	UPROPERTY (Replicated)
	TArray <int> _takenStartingZones;

	UPROPERTY (Replicated)
	bool _gameStarted = false;

	UPROPERTY (Replicated)
	int _stage = 1;

	UPROPERTY ()
	UGameStageInfo* _gameStageInfo;
	UPROPERTY ()
	UPlayerProgressionInfo* _playerProgressionInfo;
	UPROPERTY ()
	APlayAreaCircle* _playAreaCircle;

	int _playerCount = 0;
	TArray <ABattleRoyalePlayerState*> _permanentDeadPlayers;

	UPROPERTY (Replicated)
	float _startTime;
	bool _gameEnded = false;

	UPROPERTY (Replicated)
	float _currentStageStartTime;

	//Scoreboard
	void AddScoreboardPlayerStats (ABattleRoyalePlayerState* playerState);

	UPROPERTY (Replicated)
	TArray <FPlayerStats> _scoreboardPlayerStats;

	UPROPERTY (Replicated)
	float _stageDurationMultiplier = 1.0f;
};
