// Copyright Sharp Raccoon 2019.

#include "ArenaPlayerController.h"
#include "Engine/World.h"
#include "PlayerStateBase.h"
#include "CharacterBase.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"

void AArenaPlayerController::BeginPlay ()
{
	//Set input mode to UI only
	SetInputUIOnly ();
}

void AArenaPlayerController::SelectSpell_Implementation (Spells spell)
{
	if (_selectedSpells.Num () >= 6)
		return;

	Cast <ACharacterBase> (GetCharacter ())->AddSpell (spell, 1, true);

	_selectedSpells.Add (spell);

	ClientSelectSpell (spell);
}

bool AArenaPlayerController::SelectSpell_Validate (Spells spell)
{
	return true;
}

void AArenaPlayerController::ClientSelectSpell_Implementation (Spells spell)
{
	_selectedSpells.Add (spell);
}

void AArenaPlayerController::RegisterGameStart ()
{
	if (_selectedSpells.Num () < 6) 
	{
		for (int i = 1; i < USpellAttributes::GetSpellCount () + 1; i++)
		{
			Spells spell = Spells (i);

			if (!_selectedSpells.Contains (spell))
				SelectSpell (spell);

			if (_selectedSpells.Num () == 6)
				break;
		}
	}

	_gameStarted = true;

	ClientRegisterGameStart ();
}

void AArenaPlayerController::ClientRegisterGameStart_Implementation ()
{
	//SetTeamNumber ();

	ShowMouseCursor (false);
	_gameStarted = true;
}

void AArenaPlayerController::SpawnPlayerCharacter (int spawnLocationIndex)
{
	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Find the correct player start based on zone spawn location index and set the player's location to that location
	for (int i = 0; i < playerStarts.Num (); i++)
	{
		if (Cast <APlayerStart> (playerStarts [i])->PlayerStartTag == FName (*FString::FromInt (spawnLocationIndex)))
			GetCharacter ()->SetActorLocation (playerStarts [spawnLocationIndex]->GetActorLocation ());
	}

	//Reset character
	ShowMouseCursor (false);
	Cast <ACharacterBase> (GetCharacter ())->ResetCharacter ();
}

Spells AArenaPlayerController::GetSelectedSpell (int index)
{
	return _selectedSpells [index];
}

int AArenaPlayerController::GetSelectedSpellsCount ()
{
	return _selectedSpells.Num ();
}
