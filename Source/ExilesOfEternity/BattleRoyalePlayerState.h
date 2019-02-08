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
	ABattleRoyalePlayerState ();

	void Tick (float DeltaTime);

	UFUNCTION (BlueprintCallable)
	float GetCurrentRedeemKillTime ();
	UFUNCTION (BlueprintCallable)
	int GetRequiredRedeemKills ();
	
	UFUNCTION (BlueprintCallable)
	bool GetPermanentDead ();

protected:
	virtual void OnKill () override;
	virtual void OnDeath () override;

private:
	void ActivateRedeemKillTimer (bool state);
	void UpdateRedeemKillTimer (float deltaTime);

	UPROPERTY (Replicated)
	float _currentRedeemKillTime = 0.0f;
	UPROPERTY (Replicated)
	int _requiredRedeemKills = 0;
	UPROPERTY (Replicated)
	bool _permanentDead = false;
};
