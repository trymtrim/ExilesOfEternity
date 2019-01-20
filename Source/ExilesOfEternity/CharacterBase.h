// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "SpellAttributes.h"
#include "CharacterBase.generated.h"

class UUIHandler;

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

	void ResetCharacter ();

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual float TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION (BlueprintImplementableEvent)
	void ServerInitializeCharacterBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void AddSpellBP (Spells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void UseSpellBP (Spells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void UseCharacterSpellBP (CharacterSpells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void CancelProjectionSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ActivateProjectionSpellBP (Spells spell, FVector location);
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ResetCharacterBP ();

	UFUNCTION (BlueprintCallable)
	bool AddSpell (Spells spell);

	UFUNCTION (BlueprintCallable)
	bool GetCanMove ();

	UFUNCTION (BlueprintCallable)
	FRotator GetAimRotation (FVector startPosition);
	UFUNCTION (BlueprintCallable)
	FVector GetAimLocation (float maxDistance, bool initialCheck);

	UPROPERTY (Replicated)
	TEnumAsByte <Spells> _currentlyActivatedSpell;

	UPROPERTY (Replicated, BlueprintReadOnly)
	float _maxHealth = 100.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _currentHealth;

	UPROPERTY (BlueprintReadOnly)
	UUIHandler* _uiHandler;

	UPROPERTY (BlueprintReadOnly)
	TArray <float> _spellCooldownPercentages = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _ultimateSpellCooldownPercentage;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _basicSpellCooldownPercentage;

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void ServerInitializeCharacter ();

	UFUNCTION (Client, Reliable)
	void ClientAddOwnedSpell (Spells spell);

	void UnlockUltimateSpell ();

	void UseSpellInput (int hotkeyIndex);
	template <int hotkeyIndex>
	void UseSpellInput ()
	{
		UseSpellInput (hotkeyIndex);
	}

	UFUNCTION (Server, Reliable, WithValidation)
	void UseSpell (Spells spell);

	//Basic and ultimate spell
	UFUNCTION (Server, Reliable, WithValidation)
	void UseCharacterSpell (CharacterSpells spell);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void UseProjectionSpell (Spells spell, FVector location);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void SetCurrentlyProjectingSpell (bool state);

	void CancelSpell ();

	void PutSpellOnCooldown (Spells spell);
	void PutSpellOnCooldown (CharacterSpells spell);

	bool GetSpellIsOnCooldown (Spells spell);
	bool GetSpellIsOnCooldown (CharacterSpells spell);

	void UpdateCooldowns (float deltaTime);

	void UpdateCooldownPercentages (float deltaTime);
	float GetCooldownPercentage (Spells spell);

	UFUNCTION (BlueprintCallable)
	void ResetCooldowns ();

	void Die ();

	UFUNCTION (Client, Reliable)
	void ClientDie ();

	UPROPERTY (Replicated)
	bool _dead;

	UCameraComponent* _cameraComponent;

	float _locationCheckMaxDistance;

	UPROPERTY (Replicated)
	bool _currentlyProjectingSpell = false;

	bool _ultimateSpellUnlocked = false;
	float _ultimateSpellCooldown = 60.0f;
	float _basicSpellCooldown = 1.0f;

	TArray <Spells> _ownedSpells;
	TMap <Spells, float> _spellCooldowns;
	TMap <CharacterSpells, float> _characterSpellCooldowns;

	UPROPERTY (Replicated)
	TArray <float> _ownedSpellsCooldownPercentages;
};
