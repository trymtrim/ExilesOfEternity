// Copyright Sharp Raccoon 2019.

#include "AICharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"

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
	}
}

void AAICharacterBase::InitializeAI (float health, float maxAggroRange)
{
	//Set health
	_maxHealth = health;
	_currentHealth = _maxHealth;

	//Set max aggro range
	_maxAggroRange = maxAggroRange;
}

void AAICharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	//Execute server specific actions
	if (GetWorld ()->IsServer ())
	{
		//If AI is out of max aggro range, retreat
		if (!_retreating && FVector::Distance (GetActorLocation (), _startLocation) > _maxAggroRange)
			Retreat ();
	}
}

void AAICharacterBase::Retreat ()
{
	//Retreat
	RetreatBP ();
	_retreating = true;

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
		Die ();
	else if (Damage > 0.0f)
		OnDamageBP (DamageCauser);
	else if (Damage < 0.0f && _currentHealth > _maxHealth)
		_currentHealth = _maxHealth;

	return Super::TakeDamage (Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AAICharacterBase::Die ()
{
	//If already dead, return;
	if (_dead)
		return;

	//Set health to zero
	_currentHealth = 0.0f;

	_dead = true;

	DieBP ();
}

void AAICharacterBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (AAICharacterBase, _maxHealth);
	DOREPLIFETIME (AAICharacterBase, _currentHealth);
	DOREPLIFETIME (AAICharacterBase, _dead);
}
