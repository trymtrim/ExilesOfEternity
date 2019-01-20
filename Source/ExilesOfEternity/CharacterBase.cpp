// Copyright Sharp Raccoon 2019.

#include "CharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "UIHandler.h"

//Sets default values
ACharacterBase::ACharacterBase ()
{
 	//Set this character to call Tick () every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Called when the game starts or when spawned
void ACharacterBase::BeginPlay ()
{
	if (IsLocallyControlled ())
	{
		//Initialize UI handler
		_uiHandler = NewObject <UUIHandler> ();

		//Get camera component
		TArray <UCameraComponent*> cameraComps;
		GetComponents <UCameraComponent> (cameraComps);
		_cameraComponent = cameraComps [0];

		//Disable mouse cursor
		ShowMouseCursor (false);
	}

	//Initialize base stats
	_currentHealth = _maxHealth;

	//GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "THIS IS A TEST YO!");

	Super::BeginPlay ();

	//Initialize server specific elements after everything else is set up
	if (IsLocallyControlled ())
		ServerInitializeCharacter ();
}

void ACharacterBase::ServerInitializeCharacter_Implementation ()
{
	ServerInitializeCharacterBP ();

	//Temp
	//AddSpell (EXAMPLE_PROJECTING_SPELL);
}

bool ACharacterBase::ServerInitializeCharacter_Validate ()
{
	return true;
}

//Called every frame
void ACharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);
}

void ACharacterBase::AddSpell (Spells spell)
{
	//If owned spells is full, return
	if (_ownedSpells.Num () == 6)
		return;

	_ownedSpells.Add (spell);
	AddSpellBP (spell);
}

void ACharacterBase::UseSpellInput (int hotkeyIndex)
{
	//If dead, return
	if (_dead)
		return;

	//If the spell is not basic or ultimate and the spell slot with the given hotkey index doesn't have a spell, return
	if (hotkeyIndex > 0)
	{
		if (_uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1] == DEFAULT_SPELL)
			return;
	}

	bool tempCurrentlyProjectingSpell = false;
	
	//If currently projecting spell, cancel projecting
	if (_currentlyProjectingSpell)
	{
		SetCurrentlyProjectingSpell (false);
		CancelProjectionSpellBP ();

		tempCurrentlyProjectingSpell = true;
	}

	if (hotkeyIndex == -1)
	{
		//Use ultimate spell
		UseCharacterSpell (ULTIMATE);
	}
	else if (hotkeyIndex == 0)
	{
		//If currently projecting a spell, activate that spell, otherwise use basic spell
		if (tempCurrentlyProjectingSpell)
			UseProjectionSpell (_currentlyActivatedSpell, GetAimLocation (USpellAttributes::GetRange (_currentlyActivatedSpell), false));
		else
			UseCharacterSpell (BASIC);
	}
	else
	{
		//TODO: Check if spell is on cooldown before using it

		//Use spell based on hotkey index
		Spells spellToUse = _uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1];

		//If not currently projecting spell and currently projected spell is not the spell to use, use spell
		if (!(tempCurrentlyProjectingSpell && _currentlyActivatedSpell == spellToUse))
			UseSpell (spellToUse);
	}
}

void ACharacterBase::UseSpell_Implementation (Spells spell)
{
	//If the spell is a projection type spell, set currently activated spell to that
	if (USpellAttributes::GetType (spell) == PROJECTION_SPELL)
	{
		_currentlyActivatedSpell = spell;
		_currentlyProjectingSpell = true;
	}

	//Use the spell, handle rest in blueprint
	UseSpellBP (spell);
}

bool ACharacterBase::UseSpell_Validate (Spells spell)
{
	//TODO: Check if spell is on cooldown before using it

	//If the player is dead or doesn't have the spell, return false
	if (_dead || !_ownedSpells.Contains (spell))
		return false;

	return true;
}

void ACharacterBase::UseCharacterSpell_Implementation (CharacterSpells spell)
{
	//TODO: Check if player has ultimate before using it
	//TODO: Check if player has basic or ultimate on cooldown before using it

	UseCharacterSpellBP (spell);
}

bool ACharacterBase::UseCharacterSpell_Validate (CharacterSpells spell)
{
	//If dead, return false
	if (_dead)
		return false;

	return true;
}

void ACharacterBase::UseProjectionSpell_Implementation (Spells spell, FVector location)
{
	ActivateProjectionSpellBP (_currentlyActivatedSpell, location);
}

bool ACharacterBase::UseProjectionSpell_Validate (Spells spell, FVector location)
{
	//If dead, return false
	if (_dead)
		return false;

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
	//If currently projecting spell, cancel it
	if (_currentlyProjectingSpell)
	{
		SetCurrentlyProjectingSpell (false);
		CancelProjectionSpellBP ();
	}
}

float ACharacterBase::TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//If dead, return
	if (_dead)
		return 0.0f;

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
	//Update death condition client-side
	ClientDie ();

	_dead = true;

	DieBP ();
}

void ACharacterBase::ClientDie_Implementation ()
{
	//Cancel current projection spell
	CancelSpell ();
}

bool ACharacterBase::GetCanMove ()
{
	//If character is dead, return false
	if (_dead)
		return false;

	return true;
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
	FVector end = start + (_cameraComponent->GetForwardVector () * 10000.0f);

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

void ACharacterBase::ShowMouseCursor (bool state)
{
	//Enable or disable mouse cursor
	GetWorld ()->GetFirstPlayerController ()->bShowMouseCursor = state;
	//GetWorld ()->GetFirstPlayerController ()->bEnableClickEvents = state;
	//GetWorld ()->GetFirstPlayerController ()->bEnableMouseOverEvents = state;

	if (state)
	{
		//Set input mode to UI
		FInputModeGameAndUI uiInputMode;
		GetWorld ()->GetFirstPlayerController ()->SetInputMode (uiInputMode);
	}
	else
	{
		//Set input mode to game
		FInputModeGameOnly gameInputMode;
		GetWorld ()->GetFirstPlayerController ()->SetInputMode (gameInputMode);
	}
}

void ACharacterBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ACharacterBase, _currentHealth);
	DOREPLIFETIME (ACharacterBase, _maxHealth);
	DOREPLIFETIME (ACharacterBase, _dead);

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

	PlayerInputComponent->BindAction ("UseSpell1", IE_Pressed, this, &ACharacterBase::UseSpellInput <1>);
	PlayerInputComponent->BindAction ("UseSpell2", IE_Pressed, this, &ACharacterBase::UseSpellInput <2>);
	PlayerInputComponent->BindAction ("UseSpell3", IE_Pressed, this, &ACharacterBase::UseSpellInput <3>);
	PlayerInputComponent->BindAction ("UseSpell4", IE_Pressed, this, &ACharacterBase::UseSpellInput <4>);
	PlayerInputComponent->BindAction ("UseSpell5", IE_Pressed, this, &ACharacterBase::UseSpellInput <5>);
	PlayerInputComponent->BindAction ("UseSpell6", IE_Pressed, this, &ACharacterBase::UseSpellInput <6>);

	PlayerInputComponent->BindAction ("ShowMouseCursor", IE_Pressed, this, &ACharacterBase::ShowMouseCursor <true>);
	PlayerInputComponent->BindAction ("ShowMouseCursor", IE_Released, this, &ACharacterBase::ShowMouseCursor <false>);
}
