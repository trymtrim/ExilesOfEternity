// Copyright Sharp Raccoon 2019.

#include "CharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "UIHandler.h"
#include "ExilesOfEternityGameModeBase.h"
#include "PlayerControllerBase.h"
#include "PlayerStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "BattleRoyalePlayerController.h"

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
		_cameraComponent = cameraComps [1];
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

		//Set basic spell charges
		_basicSpellCharges = _maxBasicSpellCharges;
		_basicSpellChargeTimer = _basicSpellChargeTime;

		//Temp
		if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "ArenaLevel" || UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "PracticeLevel")
			level = 6;
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

		//Update item usage
		if (GetCurrentlyUsingItem ())
			UpdateUsingItem (DeltaTime);

		//If character is under world bounds, teleport to center of map
		if (GetActorLocation ().Z < -20000.0f)
		{
			if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "GameLevel")
				SetActorLocation (FVector (38710.0f, 61300.0f, 1330.0f));
			else if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "ArenaLevel" || UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "PracticeLevel")
				SetActorLocation (FVector (0.0f, 0.0f, 5000.0f));
		}
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

bool ACharacterBase::AddSpell (Spells spell, int rank, bool hack)
{
	//If owned spells is full, return
	if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) == "GameLevel")
	{
		if (_ownedSpells.Num () == 6 || _ownedSpells.Contains (spell) || _spellSlots <= _ownedSpells.Num ())
		{
			//Add error message
			if (_ownedSpells.Contains (spell))
				Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have that ability", true);
			else
				Cast <APlayerControllerBase> (GetController ())->AddMessage ("Not enough ability slots", true);

			//Temp solution propably
			if (_spellSlots <= _ownedSpells.Num () && _ownedSpells.Num () != 6 && !_ownedSpells.Contains (spell))
			{
				if (!hack)
					return false;
			}
			else
				return false;
		}
	}
	else
	{
		if (_ownedSpells.Num () == 6 || _ownedSpells.Contains (spell))
		{
			//Add error message
			if (_ownedSpells.Contains (spell))
				Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have that ability", true);
			else
				Cast <APlayerControllerBase> (GetController ())->AddMessage ("Not enough ability slots", true);

			return false;
		}
	}

	//Add unlock message
	//Cast <APlayerControllerBase> (GetController ())->AddMessage ("You unlocked " + USpellAttributes::GetName (spell), false);

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
	ClientAddOwnedSpell (spell, rank);

	//Set spell rank
	if (rank == 0)
		_spellRanks.Add (spell, 1);
	else
		_spellRanks.Add (spell, rank);

	return true;
}

void ACharacterBase::ClientAddOwnedSpell_Implementation (Spells spell, int rank)
{
	//Add spell
	_ownedSpells.Add (spell);
	_globalCooldownsActivated.Add (spell, false);

	//Set spell rank
	if (rank == 0)
		_spellRanks.Add (spell, 1);
	else
		_spellRanks.Add (spell, rank);
}

void ACharacterBase::UpgradeSpell_Implementation (Spells spell)
{
	//If player doesn't have spell, return
	if (!_spellRanks.Contains (spell))
		return;

	//If spell already is at max rank, return
	if (_spellRanks [spell] == 3 || _spellUpgradesAvailable == 0)
		return;

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

	ClientUpgradeSpellBP ();
}

void ACharacterBase::AddSpellUpgrade ()
{
	_spellUpgradesAvailable++;
}

void ACharacterBase::DropSpell_Implementation (Spells spell)
{
	if (!_ownedSpells.Contains (spell))
		return;

	//Declare spawn parameters
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector spawnPosition = GetActorLocation () - FVector (0.0f, 0.0f, 88.0f);
	FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);
	spawnParams.Owner = this;

	//Spawn spell capsule
	GetWorld ()->SpawnActor <AActor> (USpellAttributes::GetSpellCapsule (spell), spawnPosition, spawnRotation, spawnParams);

	//Remove spell from owned spells
	_ownedSpells.Remove (spell);
	_spellRanks.Remove (spell);

	//Update spell drop client-side
	ClientDropSpell (spell);

	UpdateCooldowns (GetWorld ()->GetDeltaSeconds (), true);
}

bool ACharacterBase::DropSpell_Validate (Spells spell)
{
	return true;
}

void ACharacterBase::ClientDropSpell_Implementation (Spells spell)
{
	_ownedSpells.Remove (spell);
	_globalCooldownsActivated.Remove (spell);
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
	//If dead, victorious, stunned or currently using ultimate spell, return
	if (_dead || _victorious || GetStunned () || _usingUltimateSpell && hotkeyIndex != -1 || GetCurrentlyUsingItem ())
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
		if (GetSpellIsOnCooldown (ULTIMATE) || !_ultimateSpellUnlocked)
			return;

		//Use ultimate spell
		UseCharacterSpell (ULTIMATE);

		//Temp
		_clientUsingUltimateSpell = true;
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
	if (spectating)
	{
		Cast <ABattleRoyalePlayerController> (GetController ())->ChangeSpectatingTarget ();

		return;
	}

	_usingBasicSpell = true;

	UpdateUsingBasicSpell ();
}

void ACharacterBase::UpdateUsingBasicSpell ()
{
	//If spell is on cooldown, or player is dead, victorious or stunned, return
	if (GetSpellIsOnCooldown (BASIC) || _dead || _victorious || GetStunned () || GetCurrentlyUsingItem ())
		return;

	//Use basic spell
	if (_maxBasicSpellCharges == 5) //Temp
		UseSpellInput (0);
	else if (!_chargingBasicSpell) //Temp
	{
		if (_currentlyProjectingSpell)
			UseSpellInput (0);
		else if (!_usingUltimateSpell)
		{
			_chargingBasicSpell = true; //Temp
			ChargeUpBasicSpellBP (); //Temp
		}
	}	
}

void ACharacterBase::StopUsingBasicSpell ()
{
	_usingBasicSpell = false;

	if (!GetSpellIsOnCooldown (BASIC) && _chargingBasicSpell) //Temp
	{
		_chargingBasicSpell = false; //Temp
		UseSpellInput (0); //Temp
	}
}

void ACharacterBase::UseSpell_Implementation (Spells spell)
{
	//If the player is dead, victorious, stunned, doesn't have the spell, the spell is on cooldown or currently using ultimate spell, return
	if (_dead || _victorious || GetStunned () ||!_ownedSpells.Contains (spell) || GetSpellIsOnCooldown (spell) || _usingUltimateSpell || GetCurrentlyUsingItem ())
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
	//If dead, victorious, the spell is on cooldown or currently using ultimate spell, return
	if (_dead || _victorious || GetSpellIsOnCooldown (spell) && !_usingUltimateSpell)
		return;

	//If the spell is ultimate and currently using ultimate spell, stop using it
	if (_usingUltimateSpell && spell == ULTIMATE)
	{
		if (_canCancelUltimate)
			StopUsingUltimateSpell (false);

		return;
	}

	//If spell is basic and there are no charges left, return
	if (spell == BASIC && _basicSpellCharges == 0)
		return;

	if (spell == ULTIMATE)
	{
		_usingUltimateSpell = true;
		_canCancelUltimate = false;

		FTimerHandle canCancelUltimateTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (canCancelUltimateTimerHandle, this, &ACharacterBase::MakeUltimateCancellable, 1.0f, false);
	}

	//Cancel current spell
	CancelCurrentSpellBP ();

	UseCharacterSpellBP (spell);

	CancelCloakBP ();

	//Put the spell on cooldown
	PutSpellOnCooldown (spell);
}

bool ACharacterBase::UseCharacterSpell_Validate (CharacterSpells spell)
{
	return true;
}

void ACharacterBase::MakeUltimateCancellable ()
{
	_canCancelUltimate = true;
}

void ACharacterBase::UseProjectionSpell_Implementation (Spells spell, FVector location)
{
	//If dead, victorious or the spell is on cooldown, return
	if (_dead || _victorious || GetSpellIsOnCooldown (spell))
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

			//Update global cooldown client-side
			ClientActivateGlobalCooldown (spellToUpdate, true);
		}
	}

	//If ultimate spell is unlocked, update ultimate spell cooldown
	if (_ultimateSpellUnlocked)
	{
		if (_characterSpellCooldowns [ULTIMATE] < _globalCooldown)
		{
			//Update cooldown
			_characterSpellCooldowns [ULTIMATE] = _globalCooldown;
			_ultimateCooldownsActivated = true;
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

void ACharacterBase::ClientActivateGlobalCooldown_Implementation (Spells spell, bool state)
{
	_globalCooldownsActivated [spell] = state;
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
		for (int i = 0; i < _ownedSpells.Num (); i++)
		{
			if (_ownedSpells [i] == spell)
			{
				if (_ownedSpellsCooldownPercentages.Num () > i)
				{
					if (_ownedSpellsCooldownPercentages [i] > 0.0f)
						return true;

					return false;
				}

				return false;
			}
		}

		return false;
	}
	else if (GetWorld ()->IsServer ())
	{
		if (_spellCooldowns [spell] <= 0.0f)
			return false;
	}

	return true;
}

bool ACharacterBase::GetCharacterSpellIsOnCooldown (CharacterSpells spell)
{
	if (spell == BASIC)
	{
		if (_basicSpellCooldownPercentage > 0.0f || _basicSpellCharges == 0)
			return true;

		return false;
	}
	else if (spell == ULTIMATE)
	{
		if (_ultimateSpellCooldownPercentage > 0.0f)
			return true;

		return false;
	}

	return true;
}

bool ACharacterBase::GetSpellIsOnGlobalCooldown (Spells spell)
{
	if (_globalCooldownsActivated.Contains (spell))
	{
		if (_globalCooldownsActivated [spell] == true)
			return true;

		return false;
	}

	return false;
}

bool ACharacterBase::GetSpellIsOnCooldown (CharacterSpells spell)
{
	if (IsLocallyControlled ())
	{
		if (spell == ULTIMATE && _usingUltimateSpell)
			return false;

		return GetCharacterSpellIsOnCooldown (spell);
	}
	else if (GetWorld ()->IsServer ())
	{
		if (_characterSpellCooldowns [spell] <= 0.0f)
			return false;
	}

	return true;
}

void ACharacterBase::UpdateCooldowns (float deltaTime, bool reset)
{
	//Update general spell cooldowns
	for (int i = 0; i < _ownedSpells.Num (); i++)
	{
		Spells spellToUpdate = _ownedSpells [i];

		//If spell is on cooldown
		if (_spellCooldowns [spellToUpdate] > 0.0f || reset)
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
				{
					_globalCooldownsActivated [spellToUpdate] = false;

					//Update global cooldown client-side
					ClientActivateGlobalCooldown (spellToUpdate, false);
				}
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
				_replicatedUltimateSpellCooldown = 0.0f;

				//If cooldown has reached zero, deactivate global cooldown
				if (_characterSpellCooldowns [ULTIMATE] <= 0.0f)
					_ultimateCooldownsActivated = false;
			}
			else
			{
				_ultimateSpellCooldownPercentage = _characterSpellCooldowns [ULTIMATE] / _ultimateSpellCooldown;
				_replicatedUltimateSpellCooldown = _characterSpellCooldowns [ULTIMATE];
			}
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
		_replicatedUltimateSpellCooldown = 0.0f;
	}

	//Handle basic spell cooldown system
	_basicSpellCharges = _maxBasicSpellCharges;
	_basicSpellChargeTimer = _basicSpellChargeTime;

	//Reset basic spell cooldown
	_characterSpellCooldowns [BASIC] = 0.0f;
	_basicSpellCooldownPercentage = 0.0f;
}

void ACharacterBase::UpgradeSpellInput (int hotkeyIndex)
{
	//If there are no spell upgrades available, return
	if (_spellUpgradesAvailable == 0)
		return;

	//Upgrade spell based on hotkey index
	Spells spellToUpgrade = _uiHandler->GetSpellPanelSpells () [hotkeyIndex - 1];

	//If spell already is at max rank, return
	if (GetSpellRank (spellToUpgrade) == 3)
		return;

	UpgradeSpell (spellToUpgrade);
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

		OnDealDamageBP (this, -(_maxHealth / 33.3f));
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

		//Call deal damage event on the damage causer
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
		{
			if (_defenseElixirActivated)
				Cast <ACharacterBase> (DamageCauser)->OnDealDamageBP (this, Damage * 0.85f);
			else
				Cast <ACharacterBase> (DamageCauser)->OnDealDamageBP (this, Damage);
		}

		if (damageCauserTeamNumber == ourTeamNumber && Damage > 0.0f)
			return 0.0f;

		//Lifesteal Stone
		ACharacterBase* damageCauserCharacter = Cast <ACharacterBase> (DamageCauser);

		if (damageCauserCharacter->GetStone () == LIFESTEAL_STONE && Damage > 0.0f)
		{
			float lifesteal = -Damage * 0.2f;

			if (damageCauserCharacter->GetCurrentHealth () < damageCauserCharacter->GetMaxHealth ())
				damageCauserCharacter->OnDealDamageBP (damageCauserCharacter, lifesteal);

			UGameplayStatics::ApplyDamage (damageCauserCharacter, lifesteal, nullptr, this, nullptr);
		}
	}

	//Delay static health regen
	_staticHealthRegenTimer = 10.0f + _staticHealthRegenTime;

	//Reduce the damage taken from current health
	if (_defenseElixirActivated)
		_currentHealth -= Damage * 0.85f;
	else
		_currentHealth -= Damage;

	//Cancel cloak elixir
	if (Damage > 0.0f)
		CancelCloakBP ();

	//If current health is zero or less, die
	if (_currentHealth <= 0.0f)
	{
		//Update kill count for player who caused the killing blow
		if (_stone != SOUL_STONE && DamageCauser->GetClass ()->IsChildOf (ACharacterBase::StaticClass ()))
			Cast <APlayerStateBase> (Cast <ACharacter> (DamageCauser)->GetPlayerState ())->AddKill (GetPlayerState ());

		Die ();
	}
	else if (Damage > 0.0f)
	{
		//Cancel stun
		if (_stunned)
			SetStunEffect (true, 0.0f);

		OnDamageBP ();

		if (_defenseElixirActivated)
			OnFloatingDamageBP (Damage * 0.85f);
		else
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

	//If player has soul stone, respawn
	if (_stone == SOUL_STONE)
	{
		_soulStoneRespawn = true;
		SetStunEffect (true, 3.0f);
		_currentHealth = _maxHealth * 0.25f;
		UseSpellBP (SHIELD);
		SoulStoneRespawnBP ();
		ClientDropItem (_stone, 0);
		_stone = EMPTY_ITEM;
		_hasStone = false;

		return;
	}

	//If defense elixir is activated, deactivate it
	if (_defenseElixirActivated)
		DeactivateDefenseElixir ();

	//Set health to zero
	_currentHealth = 0.0f;
	
	//Update death count
	Cast <APlayerStateBase> (GetPlayerState ())->AddDeath ();

	//Update death condition client-side
	ClientDie ();

	_dead = true;

	//Reset spell effects
	ResetSpellEffects ();

	DieBP ();
	
	//Cancel current spell
	CancelCurrentSpellBP ();

	//Cancel basic spell charge - Gideon
	CancelBasicSpellChargeBP ();

	Cast <AExilesOfEternityGameModeBase> (GetWorld ()->GetAuthGameMode ())->ReportDeath (this);

	//Cancel using item
	CancelUsingItem ();

	//Drop items
	if (_stone != EMPTY_ITEM)
		DropItem (_stone);

	int tempFirstItemAmount = _firstItemAmount;
	int tempSecondItemAmount = _secondItemAmount;

	for (int i = 0; i < tempFirstItemAmount; i++)
		DropItem (_firstItem);

	for (int i = 0; i < tempSecondItemAmount; i++)
		DropItem (_secondItem);
}

void ACharacterBase::ClientDie_Implementation ()
{
	_chargingBasicSpell = false;

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

void ACharacterBase::StopUsingUltimateSpell (bool finished)
{
	_usingUltimateSpell = false;
	StopUsingUltimateSpellBP (finished);
	
	ClientStopUsingUltimateSpell ();
}

void ACharacterBase::ClientStopUsingUltimateSpell_Implementation ()
{
	//Temp
	_clientUsingUltimateSpell = false;
}

void ACharacterBase::MakeVictorious ()
{
	_victorious = true;
}

void ACharacterBase::SetMovingSpell (bool state)
{
	_currentlyMovingSpell = state;
}

float ACharacterBase::GetBasicSpellDamage ()
{
	return _basicSpellDamage;
}

bool ACharacterBase::GetCanMove ()
{
	//If character is dead, stunned or has won the game, return false
	if (_dead || _stunned || _victorious)
		return false;

	//Temp
	if (_clientUsingUltimateSpell && _maxBasicSpellCharges != 5)
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

bool ACharacterBase::GetUsingUltimateSpell ()
{
	return _usingUltimateSpell;
}

bool ACharacterBase::GetDead ()
{
	return _dead;
}

bool ACharacterBase::GetMovingSpell ()
{
	return _currentlyMovingSpell;
}

int ACharacterBase::GetSpellSlots ()
{
	return _spellSlots;
}

float ACharacterBase::GetCurrentHealth ()
{
	return _currentHealth;
}

float ACharacterBase::GetMaxHealth ()
{
	return _maxHealth;
}

int ACharacterBase::GetSpellSlotIndex ()
{
	_currentSpellSlotIndex++;

	return _currentSpellSlotIndex;
}

void ACharacterBase::ResetSpellSlotIndex ()
{
	_currentSpellSlotIndex = -1;
}

void ACharacterBase::SetSlowEffect (float value, float duration)
{
	_slowEffect = value;

	if (value == 1.0f)
		_slowed = false;
	else
	{
		_slowed = true;
		SetSlowEffectBP (value, duration);
	}
}

float ACharacterBase::GetSlowEffect ()
{
	return _slowEffect;
}

bool ACharacterBase::GetSlowed ()
{
	return _slowed;
}

void ACharacterBase::SetStunEffect (bool state, float duration)
{
	_stunned = state;

	StopUsingBasicSpell ();

	CancelUsingItem ();

	if (state)
		SetStunEffectBP (duration);
}

bool ACharacterBase::GetStunned ()
{
	return _stunned;
}

void ACharacterBase::ResetSpellEffects ()
{
	_slowed = false;
	_slowEffect = 1.0f;

	_stunned = false;
}

void ACharacterBase::AddSpellSlot ()
{
	if (_spellSlots == 6)
		return;

	_spellSlots++;
}

bool ACharacterBase::AddItem (Items item)
{
	//If player is dead, return
	if (_dead)
		return false;

	//If item is stone
	if (USpellAttributes::GetItemStone (item))
	{
		//If player doesn't have a stone already, add item as stone
		if (_stone == EMPTY_ITEM)
		{
			_stone = item;
			_hasStone = true;
			ClientAddItem (item, 0);

			//Add item message - currently in blueprint

			//Constitution Stone
			if (item == CONSTITUTION_STONE)
			{
				float currentHealthPercentage = _currentHealth / _maxHealth;

				_maxHealth += 40.0f;
				_currentHealth = _maxHealth * currentHealthPercentage;
			}

			return true;
		}
		else
		{
			//Add stone error message
			Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have a stone in your inventory", true);

			return false;
		}
	}
	else //If item is regular item
	{
		//If player doesn't have the item already, add item
		if (_firstItem != item && _secondItem != item)
		{
			if (_firstItem == EMPTY_ITEM)
			{
				_firstItem = item;
				_firstItemAmount = 1;
				ClientAddItem (item, 1);

				//Add item message - currently in blueprint

				return true;
			}
			else if (_secondItem == EMPTY_ITEM)
			{
				_secondItem = item;
				_secondItemAmount = 1;
				ClientAddItem (item, 2);

				//Add item message - currently in blueprint

				return true;
			}
			else
			{
				//Add item error message
				Cast <APlayerControllerBase> (GetController ())->AddMessage ("Your inventory is full", true);

				return false;
			}
		}
		else //If player has the item already
		{
			if (_firstItem == item)
			{
				//If inventory is not full, add item
				if (_firstItemAmount < 3)
				{
					_firstItemAmount++;
					ClientAddItem (item, 1);

					//Add item message - currently in blueprint

					return true;
				}
				else
				{
					//Add item error message
					Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have 3 of that item", true);

					return false;
				}
			}
			else if (_secondItem == item)
			{
				//If inventory is not full, add item
				if (_secondItemAmount < 3)
				{
					_secondItemAmount++;
					ClientAddItem (item, 2);

					//Add item message - currently in blueprint

					return true;
				}
				else
				{
					//Add item error message
					Cast <APlayerControllerBase> (GetController ())->AddMessage ("You already have 3 of that item", true);

					return false;
				}
			}
		}
	}

	return false;
}

void ACharacterBase::ClientAddItem_Implementation (Items item, int slot)
{
	switch (slot)
	{
	case 0:
		_stone = item;
		break;
	case 1:
		_firstItem = item;
		break;
	case 2:
		_secondItem = item;
		break;
	}
}

void ACharacterBase::DropItem_Implementation (Items item)
{
	if (_stone != item && _firstItem != item && _secondItem != item)
		return;

	//Declare spawn parameters
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector spawnPosition = GetActorLocation () - FVector (0.0f, 0.0f, 88.0f);
	FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);
	spawnParams.Owner = this;

	//Spawn spell capsule
	GetWorld ()->SpawnActor <AActor> (USpellAttributes::GetItemBlueprint (item), spawnPosition, spawnRotation, spawnParams);

	if (_stone == item)
	{
		//Constitution Stone
		if (item == CONSTITUTION_STONE)
		{
			float currentHealthPercentage = _currentHealth / _maxHealth;

			_maxHealth -= 40.0f;
			_currentHealth = _maxHealth * currentHealthPercentage;
		}

		_stone = EMPTY_ITEM;
		_hasStone = false;

		ClientDropItem (item, 0);
	}
	else if (_firstItem == item)
	{
		_firstItemAmount--;

		if (_firstItemAmount == 0)
			_firstItem = EMPTY_ITEM;

		ClientDropItem (item, _firstItemAmount);
	}
	else if (_secondItem == item)
	{
		_secondItemAmount--;

		if (_secondItemAmount == 0)
			_secondItem = EMPTY_ITEM;

		ClientDropItem (item, _secondItemAmount);
	}
}

bool ACharacterBase::DropItem_Validate (Items item)
{
	return true;
}

void ACharacterBase::ClientDropItem_Implementation (Items item, int remainingAmount)
{
	if (_stone == item)
		_stone = EMPTY_ITEM;
	else if (_firstItem == item)
	{
		if (remainingAmount == 0)
			_firstItem = EMPTY_ITEM;
	}
	else if (_secondItem == item)
	{
		if (remainingAmount == 0)
			_secondItem = EMPTY_ITEM;
	}
}

void ACharacterBase::SwitchItemPosition_Implementation ()
{
	Items tempFirstItem = _firstItem;
	int tempFirstItemAmount = _firstItemAmount;

	_firstItem = _secondItem;
	_firstItemAmount = _secondItemAmount;

	_secondItem = tempFirstItem;
	_secondItemAmount = tempFirstItemAmount;

	ClientSwitchItemPosition ();
}

bool ACharacterBase::SwitchItemPosition_Validate ()
{
	return true;
}

void ACharacterBase::ClientSwitchItemPosition_Implementation ()
{
	Items tempFirstItem = _firstItem;

	_firstItem = _secondItem;
	_secondItem = tempFirstItem;
}

void ACharacterBase::UseFirstItemInput ()
{
	if (_dead || _victorious || GetStunned () || _usingUltimateSpell || GetCurrentlyUsingItem ())
		return;

	//If item is empty, return
	if (_firstItem == EMPTY_ITEM || _firstItemAmount == 0)
		return;

	UseItem (1);
}

void ACharacterBase::UseSecondItemInput ()
{
	if (_dead || _victorious || GetStunned () || _usingUltimateSpell || GetCurrentlyUsingItem ())
		return;

	//If item is empty, return
	if (_secondItem == EMPTY_ITEM || _secondItemAmount == 0)
		return;

	UseItem (2);
}

void ACharacterBase::UseItem_Implementation (int slot)
{
	if (_dead || _victorious || GetStunned () || _usingUltimateSpell || GetCurrentlyUsingItem ())
		return;

	//Stop using basic spell
	StopUsingBasicSpell ();

	if (slot == 1)
	{
		//If item is empty, return
		if (_firstItem == EMPTY_ITEM)
			return;

		if (USpellAttributes::GetItemUseTime (_firstItem) > 0.0f)
		{
			_currentItemTimer = USpellAttributes::GetItemUseTime (_firstItem);
			_currentlyUsedItemIndex = 1;

			UsePotionOrElixirBP (_firstItem);
		}
		else
		{
			UseItemBP (_firstItem);

			_firstItemAmount--;

			if (_firstItemAmount == 0)
				_firstItem = EMPTY_ITEM;
		}
	}
	else if (slot == 2)
	{
		//If item is empty, return
		if (_secondItem == EMPTY_ITEM)
			return;

		if (USpellAttributes::GetItemUseTime (_secondItem) > 0.0f)
		{
			_currentItemTimer = USpellAttributes::GetItemUseTime (_secondItem);
			_currentlyUsedItemIndex = 2;

			UsePotionOrElixirBP (_secondItem);
		}
		else
		{
			UseItemBP (_secondItem);

			_secondItemAmount--;

			if (_secondItemAmount == 0)
				_secondItem = EMPTY_ITEM;
		}
	}
}

bool ACharacterBase::UseItem_Validate (int slot)
{
	return true;
}

void ACharacterBase::UpdateUsingItem (float deltaTime)
{
	if (_currentlyUsedItemIndex == 1)
	{
		if (_firstItem == EMPTY_ITEM || _firstItemAmount == 0)
		{
			_currentlyUsedItemIndex = 0;
			_currentItemTimer = 0.0f;
			return;
		}
	}
	else
	{
		if (_secondItem == EMPTY_ITEM || _secondItemAmount == 0)
		{
			_currentlyUsedItemIndex = 0;
			_currentItemTimer = 0.0f;
			return;
		}
	}

	_currentItemTimer -= deltaTime;

	if (_currentItemTimer <= 0.0f)
		FinishUsingItem ();
}

void ACharacterBase::FinishUsingItem ()
{
	if (_currentlyUsedItemIndex == 1)
	{
		UseItemBP (_firstItem);

		StopUsingPotionOrElixirBP (_firstItem);

		_firstItemAmount--;

		if (_firstItemAmount == 0)
			_firstItem = EMPTY_ITEM;
	}
	else
	{
		UseItemBP (_secondItem);

		StopUsingPotionOrElixirBP (_secondItem);

		_secondItemAmount--;

		if (_secondItemAmount == 0)
			_secondItem = EMPTY_ITEM;
	}

	_currentItemTimer = 0.0f;
	_currentlyUsedItemIndex = 0;
}

void ACharacterBase::CancelUsingItem ()
{
	if (_currentlyUsedItemIndex == 1)
		StopUsingPotionOrElixirBP (_firstItem);
	else if (_currentlyUsedItemIndex == 2)
		StopUsingPotionOrElixirBP (_secondItem);

	_currentItemTimer = 0.0f;
	_currentlyUsedItemIndex = 0;
}

void ACharacterBase::RegainHealth (int percent)
{
	float healthRegain = _maxHealth * (float) percent / 100.0f;
	_currentHealth += healthRegain;

	if (_currentHealth > _maxHealth)
		_currentHealth = _maxHealth;

	OnDealDamageBP (this, -healthRegain);
}

void ACharacterBase::ActivateDefenseElixir ()
{
	_defenseElixirActivated = true;
}

void ACharacterBase::DeactivateDefenseElixir ()
{
	_defenseElixirActivated = false;
}

bool ACharacterBase::GetDefenseElixirActivated ()
{
	return _defenseElixirActivated;
}

Items ACharacterBase::GetFirstItem ()
{
	return _firstItem;
}

Items ACharacterBase::GetSecondItem ()
{
	return _secondItem;
}

Items ACharacterBase::GetStone ()
{
	return _stone;
}

int ACharacterBase::GetFirstItemAmount ()
{
	return _firstItemAmount;
}

int ACharacterBase::GetSecondItemAmount ()
{
	return _secondItemAmount;
}

bool ACharacterBase::GetHasStone ()
{
	return _hasStone;
}

bool ACharacterBase::GetCurrentlyUsingItem ()
{
	if (_currentlyUsedItemIndex == 0)
		return false;

	return true;
}

float ACharacterBase::GetCurrentItemTimerPercentage ()
{
	if (_currentlyUsedItemIndex == 1)
		return 1 - _currentItemTimer / USpellAttributes::GetItemUseTime (_firstItem);

	return 1 - _currentItemTimer / USpellAttributes::GetItemUseTime (_secondItem);
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

FVector ACharacterBase::GetTeleportAimLocation (float maxDistance)
{
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

	aimLocation += FVector (0.0f, 0.0f, 88.0f);

	aimLocation -= _cameraComponent->GetForwardVector () * 60.0f;

	return aimLocation;
}

AActor* ACharacterBase::GetActorAimedAt (float maxDistance)
{
	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = start + (_cameraComponent->GetForwardVector () * maxDistance);

	//If line trace hits anything, return the actor hit
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
		return hit.Actor.Get ();

	return nullptr;
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
	DOREPLIFETIME_CONDITION (ACharacterBase, _replicatedUltimateSpellCooldown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellCooldownPercentage, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellCharges, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _basicSpellChargeTimer, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _usingUltimateSpell, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION (ACharacterBase, _spellSlots, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION (ACharacterBase, _firstItemAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _secondItemAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _hasStone, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _currentlyUsedItemIndex, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION (ACharacterBase, _currentItemTimer, COND_OwnerOnly);

	DOREPLIFETIME (ACharacterBase, _soulStoneRespawn);
	DOREPLIFETIME (ACharacterBase, _defenseElixirActivated);

	DOREPLIFETIME_CONDITION (ACharacterBase, _victorious, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION (ACharacterBase, _slowEffect, COND_OwnerOnly);
	DOREPLIFETIME (ACharacterBase, _slowed);
	DOREPLIFETIME (ACharacterBase, _stunned);
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

	PlayerInputComponent->BindAction ("UpgradeSpell_1", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <1>);
	PlayerInputComponent->BindAction ("UpgradeSpell_2", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <2>);
	PlayerInputComponent->BindAction ("UpgradeSpell_3", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <3>);
	PlayerInputComponent->BindAction ("UpgradeSpell_4", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <4>);
	PlayerInputComponent->BindAction ("UpgradeSpell_5", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <5>);
	PlayerInputComponent->BindAction ("UpgradeSpell_6", IE_Pressed, this, &ACharacterBase::UpgradeSpellInput <6>);

	PlayerInputComponent->BindAction ("UseItem1", IE_Pressed, this, &ACharacterBase::UseFirstItemInput);
	PlayerInputComponent->BindAction ("UseItem2", IE_Pressed, this, &ACharacterBase::UseSecondItemInput);
}
