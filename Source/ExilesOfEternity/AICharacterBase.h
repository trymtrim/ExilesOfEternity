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

	UFUNCTION (BlueprintImplementableEvent)
	void OnDamageBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();

	UPROPERTY (Replicated, BlueprintReadOnly)
	float _maxHealth = 50.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _currentHealth;

private:
	void Die ();

	UPROPERTY (Replicated)
	bool _dead = false;
};
