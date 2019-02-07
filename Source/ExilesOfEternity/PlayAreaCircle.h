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

	virtual void Tick (float DeltaTime) override;

	void StartShrinking (int stage);

protected:
	virtual void BeginPlay () override;

private:
	void UpdateShrinking (float deltaTime);
	void CheckPlayersOutsidePlayArea ();

	int _stage = 1;
	FVector _endPosition;
	bool _currentlyShrinking = false;

	UGameStageInfo* _gameStageInfo;
};
