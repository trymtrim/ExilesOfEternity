// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayAreaCircle.generated.h"

class UGameStageInfo;

UCLASS()
class EXILESOFETERNITY_API APlayAreaCircle : public AActor
{
	GENERATED_BODY ()
	
public:	
	APlayAreaCircle ();

	UFUNCTION (BlueprintCallable)
	void LoadGameStageInfo (UGameStageInfo* gameStageInfo);

	virtual void Tick (float DeltaTime) override;

	void StartShrinking (int stage);

	bool GetActorInsidePlayArea (AActor* actor);
	FVector GetEndLocation ();

private:
	void UpdateShrinking (float deltaTime);
	void CheckPlayersOutsidePlayArea ();

	UPROPERTY (Replicated)
	int _stage = 1;
	bool _currentlyShrinking = false;
	FVector _startLocation;
	UPROPERTY (Replicated)
	FVector _endLocation;
	float _totalMoveTime = 0.0f;

	UGameStageInfo* _gameStageInfo;
};
