// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "BattleRoyaleGameState.generated.h"

class APlayAreaCircle;

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
};

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameState : public AGameStateBase
{
	GENERATED_BODY ()

public:
	void SetStartingZoneTaken (int zoneIndex);
	void StartGame ();
	void StartNextStage ();

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
	UPROPERTY (Replicated)
	TArray <int> _takenStartingZones;

	UPROPERTY (Replicated)
	bool _gameStarted = false;

	int _stage = 0;

	UGameStageInfo* _gameStageInfo;
	APlayAreaCircle* _playAreaCircle;
};
