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

	UFUNCTION (BlueprintCallable)
	bool AddSpell (Spells spell, int rank, bool hack); //REMINDER: Remove hack later

	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void DropSpell (Spells spell);
	void AddSpellUpgrade ();
	UFUNCTION (BlueprintCallable)
	void UnlockUltimateSpell ();

	UFUNCTION (BlueprintCallable)
	void SetImmunity (bool state);

	UFUNCTION (Client, Reliable)
	void ClientHandleRespawn ();

	UFUNCTION (BlueprintCallable)
	void StopUsingUltimateSpell (bool finished);
	UFUNCTION (Client, Reliable)
	void ClientStopUsingUltimateSpell ();

	void MakeVictorious ();

	UFUNCTION (BlueprintCallable)
	void SetMovingSpell (bool state);

	UFUNCTION (BlueprintCallable)
	int GetSpellCount ();
	UFUNCTION (BlueprintCallable)
	int GetSpellUpgradesAvailable ();
	UFUNCTION (BlueprintCallable)
	bool GetUltimateSpellUnlocked ();
	UFUNCTION (BlueprintCallable)
	bool GetUsingUltimateSpell ();
	UFUNCTION (BlueprintCallable)
	bool GetDead ();

	UFUNCTION (BlueprintCallable)
	bool GetMovingSpell ();

	UFUNCTION (BlueprintCallable)
	bool GetSpellIsOnCooldown (Spells spell);
	UFUNCTION (BlueprintCallable)
	bool GetCharacterSpellIsOnCooldown (CharacterSpells spell);
	UFUNCTION (BlueprintCallable)
	bool GetSpellIsOnGlobalCooldown (Spells spell);

	UFUNCTION (BlueprintCallable)
	int GetSpellSlots ();
	
	float GetCurrentHealth ();
	float GetMaxHealth ();

	UFUNCTION (BlueprintCallable)
	int GetSpellSlotIndex ();
	UFUNCTION (BlueprintCallable)
	void ResetSpellSlotIndex ();

	UFUNCTION (BlueprintImplementableEvent)
	void OnDealDamageBP (AActor* actor, float damage);

	//Spell effects
	UFUNCTION (BlueprintCallable)
	void SetSlowEffect (float value, float duration); //Default = 1.0f
	UFUNCTION (BlueprintCallable)
	float GetSlowEffect ();
	UFUNCTION (BlueprintCallable)
	bool GetSlowed ();
	UFUNCTION (BlueprintCallable)
	void SetStunEffect (bool state, float duration);
	UFUNCTION (BlueprintCallable)
	bool GetStunned ();

	void ResetSpellEffects ();

	void AddSpellSlot ();

	//Items
	UFUNCTION (BlueprintCallable)
	bool AddItem (Items item);
	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void DropItem (Items item);
	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void SwitchItemPosition ();

	UFUNCTION (BlueprintCallable)
	Items GetFirstItem ();
	UFUNCTION (BlueprintCallable)
	Items GetSecondItem ();
	UFUNCTION (BlueprintCallable)
	Items GetStone ();
	UFUNCTION (BlueprintCallable)
	int GetFirstItemAmount ();
	UFUNCTION (BlueprintCallable)
	int GetSecondItemAmount ();
	UFUNCTION (BlueprintCallable)
	bool GetHasStone ();
	UFUNCTION (BlueprintCallable)
	bool GetCurrentlyUsingItem ();
	UFUNCTION (BlueprintCallable)
	float GetCurrentItemTimerPercentage ();

	//Used for specific game modes
	UFUNCTION (BlueprintImplementableEvent)
	void GainExperienceBP (int experience);
	void LevelUp (int newLevel);
	UFUNCTION (BlueprintImplementableEvent)
	void OnLevelUpBP (int level);
	int level = 1;

	UPROPERTY (BlueprintReadOnly)
	ACharacterBase* spectatingTarget = nullptr;
	UPROPERTY (BlueprintReadOnly)
	bool spectating = false;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual float TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION (BlueprintImplementableEvent)
	void ServerInitializeCharacterBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void AddSpellBP (Spells spell);
	UFUNCTION (BlueprintImplementableEvent)
	void RemoveSpellBP (Spells spell);
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
	void OnFloatingDamageBP (float Damage);
	UFUNCTION (BlueprintImplementableEvent)
	void DieBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void ResetCharacterBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void HandleRespawnBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void StopUsingUltimateSpellBP (bool finished);
	UFUNCTION (BlueprintImplementableEvent)
	void ClientUpgradeSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void UseItemBP (Items item);

	//Spell effects
	UFUNCTION (BlueprintImplementableEvent)
	void SetSlowEffectBP (float value, float duration);
	UFUNCTION (BlueprintImplementableEvent)
	void SetStunEffectBP (float duration);

	//Temp
	UFUNCTION (BlueprintImplementableEvent)
	void ChargeUpBasicSpellBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void CancelBasicSpellChargeBP ();

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
	UFUNCTION (BlueprintCallable)
	FVector GetTeleportAimLocation (float maxDistance);
	UFUNCTION (BlueprintCallable)
	AActor* GetActorAimedAt (float maxDistance);

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
	float _replicatedUltimateSpellCooldown;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _basicSpellCooldownPercentage;

	//Items
	UFUNCTION (BlueprintCallable)
	void RegainHealth (int percent);
	UFUNCTION (BlueprintCallable)
	void ActivateDefenseElixir ();
	UFUNCTION (BlueprintCallable)
	void DeactivateDefenseElixir ();
	UFUNCTION (BlueprintCallable)
	bool GetDefenseElixirActivated ();

	UFUNCTION (BlueprintImplementableEvent)
	void SoulStoneRespawnBP ();
	UFUNCTION (BlueprintImplementableEvent)
	void CancelCloakBP ();

	UPROPERTY (Replicated, BlueprintReadOnly)
	int _currentlyUsedItemIndex = 0;
	UPROPERTY (Replicated, BlueprintReadWrite)
	bool _soulStoneRespawn = false;

	//Basic spell charging
	UPROPERTY (BlueprintReadOnly, EditAnywhere)
	int _maxBasicSpellCharges = 5;
	UPROPERTY (Replicated, BlueprintReadOnly)
	int _basicSpellCharges = 5;
	UPROPERTY (BlueprintReadOnly, EditAnywhere)
	float _basicSpellChargeTime = 2.0f;
	UPROPERTY (Replicated, BlueprintReadOnly)
	float _basicSpellChargeTimer = 2.0f;

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void ServerInitializeCharacter ();

	UFUNCTION (Client, Reliable)
	void ClientAddOwnedSpell (Spells spell, int rank);
	UFUNCTION (Client, Reliable)
	void ClientUpgradeSpell (Spells spell);
	UFUNCTION (Client, Reliable)
	void ClientDropSpell (Spells spell);

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
	UFUNCTION (BlueprintCallable)
	void StopUsingBasicSpell ();

	//Basic and ultimate spell
	UFUNCTION (Server, Reliable, WithValidation)
	void UseCharacterSpell (CharacterSpells spell);

	void MakeUltimateCancellable ();

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
	UFUNCTION (Client, Reliable)
	void ClientActivateGlobalCooldown (Spells spell, bool state);

	bool GetSpellIsOnCooldown (CharacterSpells spell);

	void UpdateCooldowns (float deltaTime, bool reset = false);

	void UpdateCooldownPercentages (float deltaTime);
	float GetCooldownPercentage (Spells spell);

	UFUNCTION (BlueprintCallable)
	void ResetCooldowns ();

	void UpgradeSpellInput (int hotkeyIndex);
	template <int hotkeyIndex>
	void UpgradeSpellInput ()
	{
		UpgradeSpellInput (hotkeyIndex);
	}

	UFUNCTION (Client, Reliable)
	void ClientDie ();

	void StaticHealthRegen (float deltaTime);

	UPROPERTY (Replicated)
	bool _dead = false;

	bool _immune = false;

	UCameraComponent* _cameraComponent;

	float _locationCheckMaxDistance;

	UPROPERTY (Replicated)
	bool _currentlyProjectingSpell = false;

	UPROPERTY (Replicated)
	bool _ultimateSpellUnlocked = false;
	UPROPERTY (EditAnywhere)
	float _ultimateSpellCooldown = 90.0f;
	UPROPERTY (EditAnywhere)
	float _basicSpellCooldown = 0.35f;

	float _globalCooldown = 0.35f;

	bool _usingBasicSpell = false;
	UPROPERTY (Replicated)
	bool _usingUltimateSpell = false;

	bool _canCancelUltimate = false;

	TMap <Spells, bool> _globalCooldownsActivated;
	bool _ultimateCooldownsActivated = false;
	bool _basicCooldownsActivated = false;

	TArray <Spells> _ownedSpells;
	TMap <Spells, float> _spellCooldowns;
	TMap <CharacterSpells, float> _characterSpellCooldowns;

	UPROPERTY (Replicated)
	TArray <float> _ownedSpellsCooldownPercentages;

	TMap <Spells, int> _spellRanks;

	float _basicSpellDamage = 15.0f;

	UPROPERTY (Replicated)
	int _spellUpgradesAvailable = 0;

	float _staticHealthRegenTime = 3.0f;
	float _staticHealthRegenTimer;

	float _currentHealing = 0.0f;

	UPROPERTY (Replicated)
	bool _victorious = false;

	int _currentSpellSlotIndex = -1;
	bool _currentlyMovingSpell = false;

	bool _chargingBasicSpell = false;
	
	UPROPERTY (Replicated)
	int _spellSlots = 2;

	//Spell effects
	UPROPERTY (Replicated)
	bool _slowed = false;
	UPROPERTY (Replicated)
	float _slowEffect = 1.0f;
	UPROPERTY (Replicated)
	bool _stunned = false;

	//Items
	UFUNCTION (Client, Reliable)
	void ClientAddItem (Items item, int slot);
	UFUNCTION (Client, Reliable)
	void ClientDropItem (Items item, int remainingAmount);
	UFUNCTION (Client, Reliable)
	void ClientSwitchItemPosition ();

	void UseFirstItemInput ();
	void UseSecondItemInput ();
	UFUNCTION (Server, Reliable, WithValidation)
	void UseItem (int slot);
	void UpdateUsingItem (float deltaTime);
	void FinishUsingItem ();
	void CancelUsingItem ();

	Items _firstItem = EMPTY_ITEM;
	Items _secondItem = EMPTY_ITEM;
	Items _stone = EMPTY_ITEM;
	UPROPERTY (Replicated)
	int _firstItemAmount = 0;
	UPROPERTY (Replicated)
	int _secondItemAmount = 0;
	UPROPERTY (Replicated)
	bool _hasStone = false;
	UPROPERTY (Replicated)
	float _currentItemTimer = 0.0f;
	UPROPERTY (Replicated)
	bool _defenseElixirActivated = false;

	//Temp
	bool _clientUsingUltimateSpell = false;
};
