// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.generated.h"

UENUM (BlueprintType)
enum Spells
{
	EXAMPLE_PROJECTING_SPELL,
	EXAMPLE_SPELL_2,
	EXAMPLE_SPELL_3
};

UENUM (BlueprintType)
enum CharacterSpells
{
	BASIC,
	ULTIMATE
};

UENUM (BlueprintType)
enum SpellAnimations
{
	PROJECTION_UPWARDS,
	PROJECTION_DOWNWARDS,
	PROJECTILE
};

UCLASS()
class EXILESOFETERNITY_API ACharacterBase : public ACharacter
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	ACharacterBase ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent (class UInputComponent* PlayerInputComponent) override;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual float TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION (BlueprintImplementableEvent)
	void UseSpellBP (Spells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void UseCharacterSpellBP (CharacterSpells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void CancelProjectionSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ActivateProjectionSpellBP (Spells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();

	UFUNCTION (BlueprintCallable)
	FRotator GetAimRotation (FVector startPosition);
	UFUNCTION (Blueprintcallable)
	FVector GetAimLocation (float maxDistance, bool initialCheck);

	UPROPERTY (Replicated, BlueprintReadOnly)
	TEnumAsByte <Spells> _currentlyActivatedSpell;

	UPROPERTY (Replicated, BlueprintReadOnly)
	float _maxHealth = 100.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _currentHealth;

private:
	void UseSpellInput (int spellIndex);

	template <int index>
	void UseSpellInput ()
	{
		UseSpellInput (index);
	}

	UFUNCTION (Server, Reliable, WithValidation)
	void UseSpell (Spells spell);

	//Basic and ultimate spell
	UFUNCTION (Server, Reliable, WithValidation)
	void UseCharacterSpell (CharacterSpells spell);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void UseProjectionSpell (Spells spell);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void SetCurrentlyProjectingSpell (bool state);

	void CancelSpell ();

	void Die ();

	UCameraComponent* _cameraComponent;

	float _locationCheckMaxDistance;

	UPROPERTY (Replicated)
	bool _currentlyProjectingSpell;
};
