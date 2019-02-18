// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacterBase.generated.h"

UCLASS()
class EXILESOFETERNITY_API AAICharacterBase : public ACharacter
{
	GENERATED_BODY ()

public:
	AAICharacterBase ();

	virtual void Tick (float DeltaTime) override;

protected:
	virtual void BeginPlay () override;

	virtual float TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION (BlueprintCallable)
	void InitializeAI (float health, float maxAggroRange);
	UFUNCTION (BlueprintCallable)
	void Retreat ();
	UFUNCTION (BlueprintCallable)
	void StopRetreating ();

	UFUNCTION (BlueprintImplementableEvent)
	void RetreatBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void OnDamageBP (AActor* damageCauser);
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();

	UPROPERTY (Replicated, BlueprintReadOnly)
	float _maxHealth = 50.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _currentHealth = 50.0f;

	UPROPERTY (BlueprintReadOnly)
	FVector _startLocation;

private:
	void RegainHealth ();
	void Die ();

	UPROPERTY (Replicated)
	bool _dead = false;

	float _maxAggroRange = 5000.0f;
	
	bool _retreating = false;
};
