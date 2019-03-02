// Copyright Sharp Raccoon 2019.

#include "CharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "UIHandler.h"
#include "ExilesOfEternityGameModeBase.h"
#include "PlayerControllerBase.h"
#include "PlayerStateBase.h"

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
	}

	//Initialize server specific elements
	if (GetWorld ()->IsServer ())
	{
		//Initialize health
		_currentHealth = _maxHealth;

		//Initialize character spell cooldowns
		_characterSpellCooldowns.Add (BASIC, 0.0f);
		_characterSpellCooldowns.Add (ULTIMATE, _ultimateSpellCooldown);

		//Set static health regen timer
		_staticHealthRegenTimer = _staticHealthRegenTime;
	}

	//GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "THIS IS A TEST YO!");

	Super::BeginPlay ();

	//Initialize server specific elements after everything else is set up
	if (IsLocallyControlled ())
		ServerInitializeCharacter ();
}

void ACharacterBase::ServerInitializeCharacter_Implementation ()
{
	ServerInitializeCharacterBP ();
}

bool ACharacterBase::ServerInitializeCharacter_Validate ()
{
	return true;
}

//Called every frame
void ACharacterBase::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	//Update owning client specific elements
	if (IsLocallyControlled ())
	{
		//Update cooldown percentages
		UpdateCooldownPercentages (DeltaTime);

		//Update using basic spell
		if (_usingBasicSpell)
			UpdateUsingBasicSpell ();
	}

	//Update server specific elements
	if (GetWorld ()->IsServer ())
	{
		//Update cooldowns
		UpdateCooldowns (DeltaTime);

		//Regain health
		StaticHealthRegen (DeltaTime);

		//If character is under world bounds, teleport to center of map
		if (GetActorLocation ().Z < -20000.0f)
			SetActorLocation (FVector (40000.0f, 40000.0f, 0.0f));
	}
}

void ACharacterBase::SetHealth (float health)
{
	_currentHealth = health;
	_maxHealth = health;
}

void ACharacterBase::SetBasicSpellDamage (float damage)
{
	_basicSpellDamage = damage;
}

bool ACharacterBase::AddSpell (Spells spell, bool hack)
{
	//If owned spells is full, return
	if (_ownedSpells.Num () == 6 || _ownedSpells.Contains (spell) || level + 1 <= _ownedSpells.Num ())
	{
		//Add error message
		if (_ownedSpells.Contains (spell))
			Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have that ability", true);
		else
			Cast <APlayerControllerBase> (GetController ())->AddMessage ("Not enough ability slots", true);

		//Temp solution propably
		if (level <= _ownedSpells.Num () && _ownedSpells.Num () != 6 && !_ownedSpells.Contains (spell))
		{
			if (!hack)
				return false;
		}
		else
			return false;
	}

	//Add unlock message
	Cast <APlayerControllerBase> (GetController ())->AddMessage ("You unlocked " + USpellAttributes::GetName (spell), false);

	//Add spell
	_ownedSpells.Add (spell);
	AddSpellBP (spell);

	if (!_spellCooldowns.Contains (spell))
	{
		//Add spell cooldown
		_spellCooldowns.Add (spell, 0.0f);
		_ownedSpellsCooldownPercentages.Add (0.0f);
		_globalCooldownsActivated.Add (spell, false);
	}

	//Add spell to owned spells client-side
	ClientAddOwnedSpell (spell);

	//Set spell rank to 1
	_spellRanks.Add (spell, 1);

	return true;
}

void ACharacterBase::ClientAddOwnedSpell_Implementation (Spells spell)
{
	//Add spell
	_ownedSpells.Add (spell);

	//Set spell rank to 1
	_spellRanks.Add (spell, 1);
}

void ACharacterBase::UpgradeSpell_Implementation (Spells spell)
{
	//If spell already is at max rank, return
	if (_spellRanks [spell] == 3 || _spellUpgradesAvailable == 0)
		return;

	//If spell rank is 2 and player level is below 5, return
	if (_spellRanks [spell] == 2 && level > 0)
	{
		if (level < 5)
			return;
	}

	//Set spell rank to one higher
	_spellRanks [spell]++;

	//Update spell upgrade client-side
	ClientUpgradeSpell (spell);

	_spellUpgradesAvailable--;

	Cast <APlayerControllerBase> (GetController ())->AddMessage (USpellAttributes::GetName (spell) + " upgraded to rank " + FString::FromInt (_spellRanks [spell]), false);
}

bool ACharacterBase::UpgradeSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::ClientUpgradeSpell_Implementation (Spells spell)
{
	//Set spell rank to one higher
	_spellRanks [spell]++;
}

void ACharacterBase::AddSpellUpgrade ()
{
	_spellUpgradesAvailable++;
}

void ACharacterBase::DropSpell_Implementation (Spells spell)
{
	if (!_ownedSpells.Contains (spell))
		return;

	//Remove spell from owned spells
	_ownedSpells.Remove (spell);
	_spellRanks.Remove (spell);

	//Update spell drop client-side
	ClientDropSpell (spell);

	//Declare spawn parameters
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector spawnPosition = GetActorLocation () - FVector (0.0f, 0.0f, 88.0f);
	FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);

	//Spawn spell capsule
	GetWorld ()->SpawnActor <AActor> (USpellAttributes::GetSpellCapsule (spell), spawnPosition, spawnRotation, spawnParams);
}

bool ACharacterBase::DropSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::ClientDropSpell_Implementation (Spells spell)
{
	_ownedSpells.Remove (spell);
	_spellRanks.Remove (spell);

	RemoveSpellBP (spell);
}

void ACharacterBase::UnlockUltimateSpell ()
{
	_ultimateSpellUnlocked = true;
	_characterSpellCooldowns [ULTIMATE] = 0.0f;
}

void ACharacterBase::UseSpellInput (int hotkeyIndex)
{
	//If dead, return
	if (_dead)
		return;

	//If the spell is not basic or ultimate and the spell slot with the given hotkey index doesn't have a spell, return
	if (hotkeyIndex > 0)
	{
		if (_uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1] == EMPTY)
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

	//If spell is not basic
	if (hotkeyIndex != 0)
	{
		//If spell is not ultimate
		if (hotkeyIndex != -1)
		{
			//Get spell based on hotkey index
			Spells spellToUse = _uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1];

			//If spell has global cooldown, stop using basic spell
			if (USpellAttributes::GetGlobalCooldown (spellToUse))
				StopUsingBasicSpell ();
		}
		else //If spell is ultimate, stop using basic spell
			StopUsingBasicSpell ();
	}

	if (hotkeyIndex == -1)
	{
		//If spell is on cooldown, return
		if (GetSpellIsOnCooldown (ULTIMATE))
			return;

		//Use ultimate spell
		UseCharacterSpell (ULTIMATE);
	}
	else if (hotkeyIndex == 0)
	{
		//If currently projecting a spell, activate that spell, otherwise use basic spell
		if (tempCurrentlyProjectingSpell)
			UseProjectionSpell (_currentlyActivatedSpell, GetAimLocation (USpellAttributes::GetRange (_currentlyActivatedSpell, GetSpellRank (_currentlyActivatedSpell)), false));
		else
		{
			//If spell is on cooldown, return
			if (GetSpellIsOnCooldown (BASIC) || _basicSpellCharges == 0)
				return;

			UseCharacterSpell (BASIC);
		}
	}
	else
	{
		//Use spell based on hotkey index
		Spells spellToUse = _uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1];

		//If spell is on cooldown, return
		if (GetSpellIsOnCooldown (spellToUse))
			return;

		//If not currently projecting spell and currently projected spell is not the spell to use, use spell
		if (!(tempCurrentlyProjectingSpell && _currentlyActivatedSpell == spellToUse))
			UseSpell (spellToUse);
	}
}

void ACharacterBase::StartUsingBasicSpell ()
{
	_usingBasicSpell = true;
}

void ACharacterBase::UpdateUsingBasicSpell ()
{
	//If spell is on cooldown, return
	if (GetSpellIsOnCooldown (BASIC))
		return;

	//Use basic spell
	UseSpellInput (0);
}

void ACharacterBase::StopUsingBasicSpell ()
{
	_usingBasicSpell = false;
}

void ACharacterBase::UseSpell_Implementation (Spells spell)
{
	//If the player is dead, doesn't have the spell or the spell is on cooldown, return
	if (_dead || !_ownedSpells.Contains (spell) || GetSpellIsOnCooldown (spell))
		return;

	//If spell is not usable while moving, check if character is moving before using spell
	if (!USpellAttributes::GetUsableWhileMoving (spell))
	{
		if (GetVelocity ().Size () > 0.1f)
			return;
	}

	//If the spell is not a projection type spell, cancel current spell
	if (USpellAttributes::GetType (spell) != PROJECTION)
		CancelCurrentSpellBP ();

	//If the spell is a projection type spell, set currently activated spell to that
	if (USpellAttributes::GetType (spell) == PROJECTION)
	{
		_currentlyActivatedSpell = spell;
		_currentlyProjectingSpell = true;
	}

	//Use the spell, handle rest in blueprint
	UseSpellBP (spell);

	//If the spell is not a projection type spell, put it on cooldown
	if (USpellAttributes::GetType (spell) != PROJECTION)
		PutSpellOnCooldown (spell);
}

bool ACharacterBase::UseSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::UseCharacterSpell_Implementation (CharacterSpells spell)
{
	//If dead or the spell is on cooldown, return
	if (_dead || GetSpellIsOnCooldown (spell))
		return;

	//If spell is basic and there are no charges left, return
	if (spell == BASIC && _basicSpellCharges == 0)
		return;

	//Cancel current spell
	CancelCurrentSpellBP ();

	UseCharacterSpellBP (spell);

	//Put the spell on cooldown
	PutSpellOnCooldown (spell);
}

bool ACharacterBase::UseCharacterSpell_Validate (CharacterSpells spell)
{
	return true;
}

void ACharacterBase::UseProjectionSpell_Implementation (Spells spell, FVector location)
{
	//If dead or the spell is on cooldown, return
	if (_dead || GetSpellIsOnCooldown (spell))
		return;

	//Cancel current spell
	CancelCurrentSpellBP ();

	ActivateProjectionSpellBP (_currentlyActivatedSpell, location);

	//Put the spell on cooldown
	PutSpellOnCooldown (spell);
}

bool ACharacterBase::UseProjectionSpell_Validate (Spells spell, FVector location)
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

void ACharacterBase::CancelSpell_Implementation ()
{
	//If currently projecting spell, cancel it
	if (_currentlyProjectingSpell)
	{
		SetCurrentlyProjectingSpell (false);
		CancelProjectionSpellBP ();
	}
}

void ACharacterBase::PutSpellOnCooldown (Spells spell)
{
	//Put spell on cooldown based on the respective spell's cooldown
	_spellCooldowns [spell] = USpellAttributes::GetCooldown (spell, GetSpellRank (spell));

	if (USpellAttributes::GetGlobalCooldown (spell))
		ActivateGlobalCooldown ();
}

void ACharacterBase::PutSpellOnCooldown (CharacterSpells spell)
{
	//If spell is ultimate, put ultimate on cooldown
	if (spell == ULTIMATE)
		_characterSpellCooldowns [spell] = _ultimateSpellCooldown;
	else //If spell is basic, put basic on cooldown
	{
		//Handle basic spell cooldown system
		_basicSpellCharges--;

		_characterSpellCooldowns [spell] = _basicSpellCooldown; //Temp float
	}

	ActivateGlobalCooldown ();
}

void ACharacterBase::ActivateGlobalCooldown ()
{
	//Update general spell cooldowns
	for (int i = 0; i < _ownedSpells.Num (); i++)
	{
		Spells spellToUpdate = _ownedSpells [i];

		if (USpellAttributes::GetGlobalCooldown (spellToUpdate) && _spellCooldowns [spellToUpdate] < _globalCooldown)
		{
			//Update cooldown
			_spellCooldowns [spellToUpdate] = _globalCooldown;
			_globalCooldownsActivated [spellToUpdate] = true;
		}
	}

	//If ultimate spell is unlocked, update ultimate spell cooldown
	if (_ultimateSpellUnlocked)
	{
		if (_characterSpellCooldowns [ULTIMATE] < _globalCooldown)
		{
			//Update cooldown
			_characterSpellCooldowns [ULTIMATE] = _globalCooldown;
			_ultimateCooldownsActivated = false;
		}
	}

	//TODO: Potentially handle basic spell cooldown system

	//Update basic spell cooldown
	if (_characterSpellCooldowns [BASIC] < _globalCooldown)
	{
		_characterSpellCooldowns [BASIC] = _globalCooldown;
		_basicCooldownsActivated = true;
	}
}

int ACharacterBase::GetSpellRank (Spells spell)
{
	if (!_spellRanks.Contains (spell))
		return 1;

	return _spellRanks [spell];
}

bool ACharacterBase::GetSpellIsOnCooldown (Spells spell)
{
	if (IsLocallyControlled ())
	{
		return false;
	}
	else if (GetWorld ()->IsServer ())
	{
		if (_spellCooldowns [spell] <= 0.0f)
			return false;
	}

	return true;
}

bool ACharacterBase::GetSpellIsOnCooldown (CharacterSpells spell)
{
	if (IsLocallyControlled ())
	{
		return false;
	}
	else if (GetWorld ()->IsServer ())
	{
		if (_characterSpellCooldowns [spell] <= 0.0f)
			return false;
	}

	return true;
}

void ACharacterBase::UpdateCooldowns (float deltaTime)
{
	//Update general spell cooldowns
	for (int i = 0; i < _ownedSpells.Num (); i++)
	{
		Spells spellToUpdate = _ownedSpells [i];

		//If spell is on cooldown
		if (_spellCooldowns [spellToUpdate] > 0.0f)
		{
			//Get the spell's original cooldown
			float originalSpellCooldown = USpellAttributes::GetCooldown (spellToUpdate, GetSpellRank (spellToUpdate));

			//If current cooldown is higher than the spell's original cooldown, set current cooldown to original cooldown
			if (_spellCooldowns [spellToUpdate] > originalSpellCooldown)
				_spellCooldowns [spellToUpdate] = originalSpellCooldown;

			//Update cooldown
			_spellCooldowns [spellToUpdate] -= deltaTime;

			//Update cooldown percentage
			if (_globalCooldownsActivated [spellToUpdate])
			{
				_ownedSpellsCooldownPercentages [i] = _spellCooldowns [spellToUpdate] / _globalCooldown;

				//If cooldown has reached zero, deactivate global cooldown
				if (_spellCooldowns [spellToUpdate] <= 0.0f)
					_globalCooldownsActivated [spellToUpdate] = false;
				else if (_currentlyProjectingSpell && _currentlyActivatedSpell == spellToUpdate)
				{
					//Prevent projection of spell when it is on cooldown, in case lag causes that to happen
					CancelSpell ();
				}
			}
			else
				_ownedSpellsCooldownPercentages [i] = _spellCooldowns [spellToUpdate] / USpellAttributes::GetCooldown (spellToUpdate, GetSpellRank (spellToUpdate));
		}
	}

	//If ultimate spell is unlocked, update ultimate spell cooldown
	if (_ultimateSpellUnlocked)
	{
		if (_characterSpellCooldowns [ULTIMATE] > 0.0f)
		{
			//Update cooldown
			_characterSpellCooldowns [ULTIMATE] -= deltaTime;

			//Update cooldown percentage
			if (_ultimateCooldownsActivated)
			{
				_ultimateSpellCooldownPercentage = _characterSpellCooldowns [ULTIMATE] / _globalCooldown;

				//If cooldown has reached zero, deactivate global cooldown
				if (_characterSpellCooldowns [ULTIMATE] <= 0.0f)
					_ultimateCooldownsActivated = false;
			}
			else
				_ultimateSpellCooldownPercentage = _characterSpellCooldowns [ULTIMATE] / _ultimateSpellCooldown;
		}
	}

	//Handle basic spell cooldown system
	if (_basicSpellCharges < _maxBasicSpellCharges)
	{
		_basicSpellChargeTimer -= deltaTime;

		if (_basicSpellChargeTimer <= 0.0f)
		{
			_basicSpellCharges++;
			_basicSpellChargeTimer = _basicSpellChargeTime;
		}
	}

	//Update basic spell cooldown
	if (_characterSpellCooldowns [BASIC] > 0.0f)
	{
		_characterSpellCooldowns [BASIC] -= deltaTime;

		//Update cooldown percentage
		if (_basicCooldownsActivated)
		{
			_basicSpellCooldownPercentage = _characterSpellCooldowns [BASIC] / _globalCooldown;

			//If cooldown has reached zero, deactivate global cooldown
			if (_characterSpellCooldowns [BASIC] <= 0.0f)
				_basicCooldownsActivated = false;
		}
		else
			_basicSpellCooldownPercentage = _characterSpellCooldowns [BASIC] / _basicSpellCooldown;
	}
}

void ACharacterBase::UpdateCooldownPercentages (float deltaTime)
{
	for (int i = 0; i < _spellCooldownPercentages.Num (); i++)
	{
		Spells spellToUpdate = _uiHandler->GetSpellPanelSpells () [i];

		if (spellToUpdate == EMPTY)
			_spellCooldownPercentages [i] = 0.0f;
		else
			_spellCooldownPercentages [i] = GetCooldownPercentage (spellToUpdate);
	}
}

float ACharacterBase::GetCooldownPercentage (Spells spell)
{
	for (int i = 0; i < _ownedSpells.Num (); i++)
	{
		if (_ownedSpells [i] == spell && _ownedSpellsCooldownPercentages.Num () > i)
			return _ownedSpellsCooldownPercentages [i];
	}

	return 0.0f;
}

void ACharacterBase::ResetCooldowns ()
{
	//Reset general spell cooldowns
	for (int i = 0; i < _ownedSpells.Num (); i++)
	{
		Spells spellToReset = _ownedSpells [i];
		_spellCooldowns [spellToReset] = 0.0f;

		_ownedSpellsCooldownPercentages [i] = 0.0f;
	}

	//If ultimate spell is unlocked, reset ultimate spell cooldown
	if (_ultimateSpellUnlocked)
	{
		_characterSpellCooldowns [ULTIMATE] = 0.0f;
		_ultimateSpellCooldownPercentage = 0.0f;
	}

	//Handle basic spell cooldown system
	_basicSpellCharges = _maxBasicSpellCharges;
	_basicSpellChargeTimer = _basicSpellChargeTime;

	//Reset basic spell cooldown
	_characterSpellCooldowns [BASIC] = 0.0f;
	_basicSpellCooldownPercentage = 0.0f;
}

void ACharacterBase::StaticHealthRegen (float deltaTime)
{
	//If character is dead, has full health, return
	if (_dead || _currentHealth >= _maxHealth)
		return;

	_staticHealthRegenTimer -= deltaTime;

	//If health regen time reaches zero
	if (_staticHealthRegenTimer <= 0.0f)
	{
		//Regain 3 % of max health
		_currentHealth += _maxHealth / 33.3f;

		//If health reached above max health, set health to max health
		if (_currentHealth > _maxHealth)
			_currentHealth = _maxHealth;

		//Reset static health regen timer
		_staticHealthRegenTimer = _staticHealthRegenTime;
	}
}

void ACharacterBase::SetImmunity (bool state)
{
	_immune = state;
}

float ACharacterBase::TakeDamage (float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Get game mode
	AExilesOfEternityGameModeBase* gameMode = Cast <AExilesOfEternityGameModeBase> (GetWorld ()->GetAuthGameMode ());

	//If dead, immune or game has ended, return, and don't heal above max health
	if (_dead || Damage > 0.0f && _immune || gameMode->GetGameEnded () || Damage < 0.0f && _currentHealth >= _maxHealth)
		return 0.0f;

	//If the damage causer is on the same team as this character, don't apply damage
	if (DamageCauser->GetClass ()->IsChildOf (ACharacterBase::StaticClass ()))
	{
		int damageCauserTeamNumber = Cast <APlayerStateBase> (Cast <ACharacter> (DamageCauser)->GetPlayerState ())->GetTeamNumber ();
		int ourTeamNumber = Cast <APlayerStateBase> (GetPlayerState ())->GetTeamNumber ();

		//Call on deal damage event on the damage causer
		if (Damage < 0.0f)
		{
			_currentHealing -= Damage;

			if (_currentHealing >= 3.0f)
			{
				Cast <ACharacterBase> (DamageCauser)->OnDealDamageBP (this, -_currentHealing);
				_currentHealing = 0.0f;
			}
		}
		else if (damageCauserTeamNumber != ourTeamNumber)
			Cast <ACharacterBase> (DamageCauser)->OnDealDamageBP (this, Damage);

		if (damageCauserTeamNumber == ourTeamNumber && Damage > 0.0f)
			return 0.0f;
	}

	//Delay static health regen
	_staticHealthRegenTimer = 10.0f + _staticHealthRegenTime;

	//Reduce the damage taken from current health
	_currentHealth -= Damage;

	//If current health is zero or less, die
	if (_currentHealth <= 0.0f)
	{
		Die ();

		//Update kill count for player who caused the killing blow
		if (DamageCauser->GetClass ()->IsChildOf (ACharacterBase::StaticClass ()))
			Cast <APlayerStateBase> (Cast <ACharacter> (DamageCauser)->GetPlayerState ())->AddKill (GetPlayerState ());
	}
	else if (Damage > 0.0f)
	{
		OnDamageBP ();
		OnFloatingDamageBP (Damage);
	}
	else if (Damage < 0.0f)
	{
		if (_currentHealth > _maxHealth)
			_currentHealth = _maxHealth;
	}

	return Super::TakeDamage (Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ACharacterBase::Die ()
{
	//If already dead, return;
	if (_dead)
		return;

	//Set health to zero
	_currentHealth = 0.0f;
	
	//Update death count
	Cast <APlayerStateBase> (GetPlayerState ())->AddDeath ();

	//Update death condition client-side
	ClientDie ();

	_dead = true;

	DieBP ();
	
	//Cancel current spell
	CancelCurrentSpellBP ();

	Cast <AExilesOfEternityGameModeBase> (GetWorld ()->GetAuthGameMode ())->ReportDeath (this);
}

void ACharacterBase::ClientDie_Implementation ()
{
	//Cancel current projection spell
	CancelSpell ();
}

void ACharacterBase::ResetCharacter ()
{
	//Reset spell cooldowns
	ResetCooldowns ();
	//Reset death
	_dead = false;
	//Reset health
	_currentHealth = _maxHealth;
	//Reset static health regen timer
	_staticHealthRegenTimer = _staticHealthRegenTime;

	ResetCharacterBP ();
}

void ACharacterBase::ClientHandleRespawn_Implementation ()
{
	HandleRespawnBP ();
}

float ACharacterBase::GetBasicSpellDamage ()
{
	return _basicSpellDamage;
}

bool ACharacterBase::GetCanMove ()
{
	//If character is dead, return false
	if (_dead)
		return false;

	return true;
}

bool ACharacterBase::GetImmunity ()
{
	return _immune;
}

int ACharacterBase::GetSpellCount ()
{
	return _ownedSpells.Num ();
}

int ACharacterBase::GetSpellUpgradesAvailable ()
{
	return _spellUpgradesAvailable;
}

bool ACharacterBase::GetUltimateSpellUnlocked ()
{
	return _ultimateSpellUnlocked;
}

bool ACharacterBase::GetDead ()
{
	return _dead;
}

void ACharacterBase::LevelUp (int newLevel)
{
	level = newLevel;

	//Call level-up blueprint event
	OnLevelUpBP (newLevel);
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
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
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
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_GameTraceChannel1, traceParams))
		aimLocation = hit.ImpactPoint;
	else //If line trace doesn't hit anything, line trace downwards to get location
	{
		start = end;
		end = end + -FVector::UpVector * maxDistance;

		if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_GameTraceChannel1, traceParams))
			aimLocation = hit.ImpactPoint;
	}

	//If distance between player and aim location is higher than max distance, check again with a closer distance
	if (FVector::Distance (_cameraComponent->GetComponentLocation (), hit.ImpactPoint) > _locationCheckMaxDistance)
	{
		float distanceCheck = 5.0f; //Make this higher to improve performance
		float newMaxDistance = maxDistance - distanceCheck;

		if (newMaxDistance < 10.0f)
			return FVector (1000000.0f, 1000000.0f, 1000000.0f);

		return GetAimLocation (newMaxDistance, false);
	}

	return aimLocation;
}

void ACharacterBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (ACharacterBase, _currentHealth);
	DOREPLIFETIME (ACharacterBase, _maxHealth);
	DOREPLIFETIME (ACharacterBase, _dead);

	DOREPLIFETIME_CONDITION (ACharacterBase, _currentlyActivatedSpell, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _currentlyProjectingSpell, COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION (ACharacterBase, _spellUpgradesAvailable, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _ultimateSpellUnlocked, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _ownedSpellsCooldownPercentages, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _ultimateSpellCooldownPercentage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellCooldownPercentage, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellCharges, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellChargeTimer, COND_OwnerOnly);
}

//Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);

	PlayerInputComponent->BindAction ("UseUltimateSpell", IE_Pressed, this, &ACharacterBase::UseSpellInput <-1>);
	PlayerInputComponent->BindAction ("UseBasicSpell", IE_Pressed, this, &ACharacterBase::StartUsingBasicSpell);
	PlayerInputComponent->BindAction ("UseBasicSpell", IE_Released, this, &ACharacterBase::StopUsingBasicSpell);
	PlayerInputComponent->BindAction ("CancelSpell", IE_Pressed, this, &ACharacterBase::CancelSpell);

	PlayerInputComponent->BindAction ("UseSpell1", IE_Pressed, this, &ACharacterBase::UseSpellInput <1>);
	PlayerInputComponent->BindAction ("UseSpell2", IE_Pressed, this, &ACharacterBase::UseSpellInput <2>);
	PlayerInputComponent->BindAction ("UseSpell3", IE_Pressed, this, &ACharacterBase::UseSpellInput <3>);
	PlayerInputComponent->BindAction ("UseSpell4", IE_Pressed, this, &ACharacterBase::UseSpellInput <4>);
	PlayerInputComponent->BindAction ("UseSpell5", IE_Pressed, this, &ACharacterBase::UseSpellInput <5>);
	PlayerInputComponent->BindAction ("UseSpell6", IE_Pressed, this, &ACharacterBase::UseSpellInput <6>);
}
