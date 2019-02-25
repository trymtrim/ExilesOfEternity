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

	void SetHealth (float health);
	void SetBasicSpellDamage (float _damage);
	UFUNCTION (BlueprintCallable)
	void Die ();
	void ResetCharacter ();

	void AddSpellUpgrade ();
	void UnlockUltimateSpell ();

	UFUNCTION (BlueprintCallable)
	void SetImmunity (bool state);

	UFUNCTION (Client, Reliable)
	void ClientHandleRespawn ();

	UFUNCTION (BlueprintCallable)
	int GetSpellCount ();
	UFUNCTION (BlueprintCallable)
	int GetSpellUpgradesAvailable ();
	UFUNCTION (BlueprintCallable)
	bool GetUltimateSpellUnlocked ();
	bool GetDead ();

	int level = 0; //Used for specific game modes

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
	void CancelCurrentSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void CancelProjectionSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ActivateProjectionSpellBP (Spells spell, FVector location);
	UFUNCTION (BlueprintImplementableEvent)
	void OnDamageBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ResetCharacterBP ();

	UFUNCTION (BlueprintImplementableEvent)
	void HandleRespawnBP ();

	UFUNCTION (BlueprintCallable)
	bool AddSpell (Spells spell);
	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void UpgradeSpell (Spells spell);

	UFUNCTION (BlueprintCallable)
	int GetSpellRank (Spells spell);

	UFUNCTION (BlueprintCallable)
	float GetBasicSpellDamage ();
	UFUNCTION (BlueprintCallable)
	bool GetCanMove ();
	UFUNCTION (BlueprintCallable)
	bool GetImmunity ();

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

	//Basic spell charging
	UPROPERTY (BlueprintReadOnly)
	int _maxBasicSpellCharges = 5;
	UPROPERTY (Replicated, BlueprintReadOnly)
	int _basicSpellCharges = 5;
	UPROPERTY (BlueprintReadOnly)
	float _basicSpellChargeTime = 2.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _basicSpellChargeTimer = 2.0f;

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void ServerInitializeCharacter ();

	UFUNCTION (Client, Reliable)
	void ClientAddOwnedSpell (Spells spell);
	UFUNCTION (Client, Reliable)
	void ClientUpgradeSpell (Spells spell);

	void UseSpellInput (int hotkeyIndex);
	template <int hotkeyIndex>
	void UseSpellInput ()
	{
		UseSpellInput (hotkeyIndex);
	}

	UFUNCTION (Server, Reliable, WithValidation)
	void UseSpell (Spells spell);

	void StartUsingBasicSpell ();
	void UpdateUsingBasicSpell ();
	void StopUsingBasicSpell ();

	//Basic and ultimate spell
	UFUNCTION (Server, Reliable, WithValidation)
	void UseCharacterSpell (CharacterSpells spell);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void UseProjectionSpell (Spells spell, FVector location);

	//Projection spells
	UFUNCTION (Server, Reliable, WithValidation)
	void SetCurrentlyProjectingSpell (bool state);

	UFUNCTION (Client, Reliable)
	void CancelSpell ();

	void PutSpellOnCooldown (Spells spell);
	void PutSpellOnCooldown (CharacterSpells spell);

	void ActivateGlobalCooldown ();

	bool GetSpellIsOnCooldown (Spells spell);
	bool GetSpellIsOnCooldown (CharacterSpells spell);

	void UpdateCooldowns (float deltaTime);

	void UpdateCooldownPercentages (float deltaTime);
	float GetCooldownPercentage (Spells spell);

	UFUNCTION (BlueprintCallable)
	void ResetCooldowns ();

	UFUNCTION (Client, Reliable)
	void ClientDie ();

	void StaticHealthRegen (float deltaTime);

	UPROPERTY (Replicated)
	bool _dead = false;

	bool _immune = false;

	UCameraComponent* _cameraComponent;

	float _locationCheckMaxDistance;
	float _locationCheckCount;

	UPROPERTY (Replicated)
	bool _currentlyProjectingSpell = false;

	UPROPERTY (Replicated)
	bool _ultimateSpellUnlocked = false;
	float _ultimateSpellCooldown = 60.0f;
	float _basicSpellCooldown = 0.35f;

	float _globalCooldown = 0.35f;

	bool _usingBasicSpell = false;

	TMap <Spells, bool> _globalCooldownsActivated;
	bool _ultimateCooldownsActivated = false;
	bool _basicCooldownsActivated = false;

	TArray <Spells> _ownedSpells;
	TMap <Spells, float> _spellCooldowns;
	TMap <CharacterSpells, float> _characterSpellCooldowns;

	UPROPERTY (Replicated)
	TArray <float> _ownedSpellsCooldownPercentages;

	TMap <Spells, int> _spellRanks;

	UPROPERTY (EditAnywhere)
	float _basicSpellDamage = 15.0f;

	UPROPERTY (Replicated)
	int _spellUpgradesAvailable = 0;

	float _staticHealthRegenTime = 3.0f;
	float _staticHealthRegenTimer;
};
