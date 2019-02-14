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

	void StartRespawnTimer (float respawnTime);
	void StopRepawnTimer ();
	void MakeVictorious ();

	UFUNCTION (BlueprintCallable)
	float GetCurrentRedeemKillTime ();
	UFUNCTION (BlueprintCallable)
	int GetRequiredRedeemKills ();
	
	UFUNCTION (BlueprintCallable)
	bool GetPermanentDead ();
	UFUNCTION (BlueprintCallable)
	bool GetVictorious ();

	UFUNCTION (BlueprintCallable)
	int GetRespawnTime ();

	UFUNCTION (BlueprintCallable)
	FString GetGameOverText ();

protected:
	virtual void OnKill () override;
	virtual void OnDeath () override;

private:
	void ActivateRedeemKillTimer (bool state);
	void UpdateRedeemKillTimer (float deltaTime);
	void DiePermanently ();
	void UpdateRespawnTimer ();

	UPROPERTY (Replicated)
	float _currentRedeemKillTime = 0.0f;
	UPROPERTY (Replicated)
	int _requiredRedeemKills = 0;
	UPROPERTY (Replicated)
	bool _permanentDead = false;
	UPROPERTY (Replicated)
	bool _victorious = false;
	UPROPERTY (Replicated)
	int _respawnTime = 0;
	UPROPERTY (Replicated)
	FString _gameOverText;
};
