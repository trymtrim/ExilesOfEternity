// Copyright Sharp Raccoon 2019.

#include "ArenaPlayerController.h"
#include "Engine/World.h"
#include "PlayerStateBase.h"
#include "CharacterBase.h"
#include "ExilesOfEternityGameInstance.h"
#include "UnrealNetwork.h"

void AArenaPlayerController::BeginPlay ()
{
	//Set input mode to UI only
	SetInputUIOnly ();

	//Initialize client specific elements
	if (!GetWorld ()->IsServer ())
		SetTeamNumber ();
}

void AArenaPlayerController::SetTeamNumber_Implementation ()
{
	if (Cast <UExilesOfEternityGameInstance> (GetGameInstance ())->arenaTeamNumber > 0)
		Cast <APlayerStateBase> (PlayerState)->SetTeamNumber (Cast <UExilesOfEternityGameInstance> (GetGameInstance ())->arenaTeamNumber);
}

bool AArenaPlayerController::SetTeamNumber_Validate ()
{
	return true;
}

void AArenaPlayerController::SelectSpell_Implementation (Spells spell)
{
	if (_selectedSpells >= 6)
		return;

	Cast <ACharacterBase> (GetCharacter ())->AddSpell (spell, 1, false);

	_selectedSpells++;
}

bool AArenaPlayerController::SelectSpell_Validate (Spells spell)
{
	return true;
}

int AArenaPlayerController::GetSelectedSpells ()
{
	return _selectedSpells;
}

void AArenaPlayerController::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (AArenaPlayerController, _selectedSpells);
}
