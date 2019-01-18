// Copyright Sharp Raccoon 2019.

#include "CharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"

//Sets default values
ACharacterBase::ACharacterBase ()
{
 	//Set this character to call Tick () every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Called when the game starts or when spawned
void ACharacterBase::BeginPlay ()
{
	Super::BeginPlay ();

	//Initialize base stats
	_currentHealth = _maxHealth;

	//Get camera component
	TArray <UCameraComponent*> cameraComps;
	GetComponents <UCameraComponent> (cameraComps);
	_cameraComponent = cameraComps [0];

	//GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "THIS IS A TEST YO!");
}

//Called every frame
void ACharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);
}

void ACharacterBase::UseSpellInput (int spellIndex)
{
	bool tempCurrentlyProjectingSpell = false;
	
	//If currently projecting spell, cancel projecting
	if (_currentlyProjectingSpell)
	{
		SetCurrentlyProjectingSpell (false);
		CancelProjectionSpellBP ();

		tempCurrentlyProjectingSpell = true;
	}

	if (spellIndex == -1)
	{
		//UseCharacterSpell (ULTIMATE);

		//Temp
		if (!(tempCurrentlyProjectingSpell && _currentlyActivatedSpell == EXAMPLE_PROJECTING_SPELL))
			UseSpell (EXAMPLE_PROJECTING_SPELL);
	}
	else if (spellIndex == 0)
	{
		if (tempCurrentlyProjectingSpell)
			UseProjectionSpell (_currentlyActivatedSpell);
		else
			UseCharacterSpell (BASIC);
	}
	else
	{
		//UseSpell (EXAMPLE_SPELL_2);
	}
}

void ACharacterBase::UseSpell_Implementation (Spells spell)
{
	if (spell == EXAMPLE_PROJECTING_SPELL) // || spell == ...
	{
		_currentlyActivatedSpell = spell;
		_currentlyProjectingSpell = true;
	}

	UseSpellBP (spell);
}

bool ACharacterBase::UseSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::UseCharacterSpell_Implementation (CharacterSpells spell)
{
	UseCharacterSpellBP (spell);
}

bool ACharacterBase::UseCharacterSpell_Validate (CharacterSpells spell)
{
	return true;
}

void ACharacterBase::UseProjectionSpell_Implementation (Spells spell)
{
	ActivateProjectionSpellBP (_currentlyActivatedSpell);
}

bool ACharacterBase::UseProjectionSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::SetCurrentlyProjectingSpell_Implementation (bool state)
{
	_currentlyProjectingSpell = state;
}

bool ACharacterBase::SetCurrentlyProjectingSpell_Validate (bool state)
{
	return true;
}

void ACharacterBase::CancelSpell ()
{
	if (_currentlyProjectingSpell)
	{
		SetCurrentlyProjectingSpell (false);
		CancelProjectionSpellBP ();
	}
}

float ACharacterBase::TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Reduce the damage taken from current health
	_currentHealth -= Damage;

	//If current health is zero or less, die
	if (_currentHealth <= 0.0f)
	{
		_currentHealth = 0.0f;

		Die ();
	}

	return Super::TakeDamage (Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ACharacterBase::Die ()
{
	DieBP ();
}

FRotator ACharacterBase::GetAimRotation (FVector startPosition)
{
	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = start + (_cameraComponent->GetForwardVector () * 50000.0f);

	FRotator aimRotation;

	//If line trace hits anything, set aim rotation towards what it hits
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Camera, traceParams))
		aimRotation = (hit.ImpactPoint - startPosition).Rotation ();
	else //If line trace doesn't hit anything, set rotation towards the end of the line trace
		aimRotation = (end - startPosition).Rotation ();

	return aimRotation;
}

FVector ACharacterBase::GetAimLocation (float maxDistance, bool initialCheck)
{
	if (initialCheck)
		_locationCheckMaxDistance = maxDistance;

	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = start + (_cameraComponent->GetForwardVector () * maxDistance);

	FVector aimLocation;

	//If line trace hits anything, set aim location to what it hits
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
		aimLocation = hit.ImpactPoint;
	else //If line trace doesn't hit anything, line trace downwards to get location
	{
		start = end;
		end = end + -FVector::UpVector * maxDistance;

		if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
			aimLocation = hit.ImpactPoint;
	}

	//If distance between player and aim location is higher than max distance, check again with a closer distance
	if (FVector::Distance (_cameraComponent->GetComponentLocation (), hit.ImpactPoint) > _locationCheckMaxDistance)
	{
		float distanceCheck = 5.0f; //Make this higher to improve performance
		float newMaxDistance = maxDistance - distanceCheck;

		return GetAimLocation (newMaxDistance, false);
	}

	return aimLocation;
}

void ACharacterBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ACharacterBase, _currentHealth);
	DOREPLIFETIME (ACharacterBase, _maxHealth);

	DOREPLIFETIME (ACharacterBase, _currentlyActivatedSpell);
	DOREPLIFETIME (ACharacterBase, _currentlyProjectingSpell);
}

//Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);

	PlayerInputComponent->BindAction ("UseUltimateSpell", IE_Pressed, this, &ACharacterBase::UseSpellInput <-1>);
	PlayerInputComponent->BindAction ("UseBasicSpell", IE_Pressed, this, &ACharacterBase::UseSpellInput <0>);
	PlayerInputComponent->BindAction ("CancelSpell", IE_Pressed, this, &ACharacterBase::CancelSpell);
}
