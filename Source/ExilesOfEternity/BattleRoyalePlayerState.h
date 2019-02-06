// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStateBase.h"
#include "BattleRoyalePlayerState.generated.h"

UCLASS()
class EXILESOFETERNITY_API ABattleRoyalePlayerState : public APlayerStateBase
{
	GENERATED_BODY ()

public:
	UFUNCTION (BlueprintCallable)
	float GetMaxRedeemTime ();
	UFUNCTION (BlueprintCallable)
	float GetCurrentRedeemTime ();

private:
	float _maxRedeemTime = 300.0f;
	float _currentRedeemTime = 0.0f;
};
