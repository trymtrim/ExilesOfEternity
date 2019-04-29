// Copyright Sharp Raccoon 2019.

#include "AICharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "CharacterBase.h"
#include "BattleRoyalePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SpellAttributes.h"

AAICharacterBase::AAICharacterBase ()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAICharacterBase::BeginPlay ()
{
	Super::BeginPlay ();

	//Initialize server specific elements
	if (GetWorld ()->IsServer ())
	{
		//Set start location
		_startLocation = GetActorLocation ();
		_originalStartLocation = _startLocation;

		if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "ArenaLevel")
			RegisterGameStart ();
	}

	//Set health
	_currentHealth = _maxHealth;
}

void AAICharacterBase::RegisterGameStart ()
{
	RegisterGameStartBP ();

	//Start level-up process
	FTimerHandle levelUpTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (levelUpTimerHandle, this, &AAICharacterBase::LevelUp, 60.0f, false);
}

void AAICharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	//Execute server specific actions
	if (GetWorld ()->IsServer ())
	{
		//Update aggro range percentage
		_aggroRangePercentage = FVector::Distance (GetActorLocation (), _startLocation) / _maxAggroRange;

		//If AI is out of max aggro range, retreat
		if (!_retreating && _aggroRangePercentage >= 1.0f)
			Retreat ();
	}
}

void AAICharacterBase::SetHasAggro (bool state)
{
	if (!_hasAggro && state)
		_startLocation = GetActorLocation ();

	_hasAggro = state;
}

void AAICharacterBase::Retreat ()
{
	//Retreat
	_retreating = true;
	_hasAggro = false;
	_startLocation = _originalStartLocation;

	RetreatBP ();

	//Regain health
	FTimerHandle regainHealthTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (regainHealthTimerHandle, this, &AAICharacterBase::RegainHealth, 1.0f, false);
}

void AAICharacterBase::RegainHealth ()
{
	//If AI is dead, return
	if (_dead)
		return;

	//Regain 20% of max health
	_currentHealth += _maxHealth / 5.0f;

	//If current health is greater than max heal, set current health to max health
	if (_currentHealth > _maxHealth)
		_currentHealth = _maxHealth;

	//Regain health again after a moment
	FTimerHandle regainHealthTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (regainHealthTimerHandle, this, &AAICharacterBase::RegainHealth, 1.0f, false);
}

void AAICharacterBase::StopRetreating ()
{
	//Set current health to max health
	_currentHealth = _maxHealth;

	//Stop retreating
	_retreating = false;

	//Clear regain health timer
	GetWorld ()->GetTimerManager ().ClearAllTimersForObject (this);
}

float AAICharacterBase::TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//If dead, return
	if (_dead)
		return 0.0f;

	//Reduce the damage taken from current health
	_currentHealth -= Damage;

	//If current health is zero or less, die
	if (_currentHealth <= 0.0f)
		Die (DamageCauser);
	else if (Damage > 0.0f)
		OnDamageBP (DamageCauser);
	else if (Damage < 0.0f && _currentHealth > _maxHealth)
		_currentHealth = _maxHealth;

	//If the damage causer is a player, call on deal damage event on the damage causer
	if (DamageCauser->GetClass ()->IsChildOf (ACharacterBase::StaticClass ()))
		Cast <ACharacterBase> (DamageCauser)->OnDealDamageBP (this, Damage);

	return Super::TakeDamage (Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AAICharacterBase::Die (AActor* damageCauser)
{
	//If already dead, return
	if (_dead)
		return;

	//Set health to zero
	_currentHealth = 0.0f;

	_dead = true;

	DieBP ();

	//If the damage causer is a player, give that player experience
	if (damageCauser->GetClass ()->IsChildOf (ACharacterBase::StaticClass ()))
	{
		ABattleRoyalePlayerState* playerState = Cast <ABattleRoyalePlayerState> (Cast <ACharacterBase> (damageCauser)->GetPlayerState ());
		
		if (playerState)
			playerState->GainExperience (_experience);
	}

	//Spawn item
	int randomNumber = FMath::RandRange (0, 99);

	if (randomNumber < 40)
	{
		TSubclassOf <AActor> spawnableItem = USpellAttributes::GetItemBlueprint (HEALTH_POTION);

		int randomLoot = FMath::RandRange (0, 4);

		switch (randomLoot)
		{
		case 0:
			spawnableItem = USpellAttributes::GetItemBlueprint (HEALTH_POTION);
			break;
		case 1:
			spawnableItem = USpellAttributes::GetItemBlueprint (RECHARGE_POTION);
			break;
		case 2:
			spawnableItem = USpellAttributes::GetItemBlueprint (LAUNCH_RUNE);
			break;
		case 3:
			spawnableItem = USpellAttributes::GetItemBlueprint (TELEPORTATION_RUNE);
			break;
		case 4:
			spawnableItem = USpellAttributes::GetItemBlueprint (CONSTITUTION_STONE);
			break;
		}

		//Declare spawn parameters
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		FVector spawnPosition = GetActorLocation () - FVector (0.0f, 0.0f, 88.0f);
		FRotator spawnRotation = GetActorRotation ();

		//Spawn item
		GetWorld ()->SpawnActor <AActor> (spawnableItem, spawnPosition, spawnRotation, spawnParams);
		
		/*for (int i = 1; i < USpellAttributes::GetItemCount () + 1; i++)
		{
			for (int j = 0; j < spellCapsuleAmountForEachSpell; j++)
			{
				TSubclassOf <AActor> spawnableSpellCapsule = USpellAttributes::GetSpellCapsule (Spells (i));

				//Declare spawn parameters
				FActorSpawnParameters spawnParams;
				spawnParams.Owner = this;
				FVector spawnPosition;
				FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);

				//Spawn spell capsule
				AActor* spellCapsule = GetWorld ()->SpawnActor <AActor> (spawnableSpellCapsule, spawnPosition, spawnRotation, spawnParams);

				//Set the spell capsule's location
				SetSpellCapsuleLocation (spellCapsule);
			}
		}*/
	}
}

void AAICharacterBase::LevelUp ()
{
	//Add one level
	_level++;

	//Add more health
	bool updateCurrentHealth = false;

	if (_currentHealth >= _maxHealth)
		updateCurrentHealth = true;

	_maxHealth += 15;

	if (updateCurrentHealth)
		_currentHealth = _maxHealth;

	//If AI has not reached max level, continue level-up process
	if (_level < 12)
	{
		FTimerHandle levelUpTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (levelUpTimerHandle, this, &AAICharacterBase::LevelUp, 60.0f, false);
	}
}

void AAICharacterBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (AAICharacterBase, _maxHealth);
	DOREPLIFETIME (AAICharacterBase, _currentHealth);
	DOREPLIFETIME (AAICharacterBase, _dead);
	DOREPLIFETIME (AAICharacterBase, _aggroRangePercentage);
	DOREPLIFETIME (AAICharacterBase, _hasAggro);
}
