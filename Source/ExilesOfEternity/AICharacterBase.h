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

	UFUNCTION (BlueprintCallable)
	void SetHasAggro (bool state);

protected:
	virtual void BeginPlay () override;

	virtual float TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

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

	UPROPERTY (Replicated, BlueprintReadOnly, EditAnyWhere)
	float _maxHealth = 50.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _currentHealth = 50.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	bool _dead = false;

	UPROPERTY (EditAnyWhere, BlueprintReadOnly)
	float _maxAggroRange = 5000.0f;
	UPROPERTY (EditAnyWhere)
	int _experience = 50;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _aggroRangePercentage = 0.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	bool _hasAggro = false;

	UPROPERTY (BlueprintReadOnly)
	FVector _startLocation;
	FVector _originalStartLocation;

	UPROPERTY (BlueprintReadOnly)
	bool _retreating = false;

private:
	void RegainHealth ();
	void Die (AActor* damageCauser);
	

	//Testing
	void LevelUp ();
	int _level = 1;
};
