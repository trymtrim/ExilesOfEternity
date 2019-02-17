// Copyright Sharp Raccoon 2019.

#include "AICharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"

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
		//Initialize base stats
		_currentHealth = _maxHealth;
	}
}

void AAICharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);
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
		OnDamageBP ();
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
