// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerBase.h"
#include "SpellAttributes.h"
#include "ArenaPlayerController.generated.h"

UCLASS()
class EXILESOFETERNITY_API AArenaPlayerController : public APlayerControllerBase
{
	GENERATED_BODY ()

public:
	UFUNCTION (Server, Reliable, WithValidation, BlueprintCallable)
	void SelectSpell (Spells spell);

	void RegisterGameStart ();
	void SpawnPlayerCharacter (int spawnLocationIndex);

	UFUNCTION (BlueprintCallable)
	Spells GetSelectedSpell (int index);
	UFUNCTION (BlueprintCallable)
	int GetSelectedSpellsCount ();

protected:
	virtual void BeginPlay () override;

private:
	//Temp
	void InitialSetTeamNumber ();

	UFUNCTION (Server, Reliable, WithValidation)
	void SetTeamNumber (int teamNumber);

	UFUNCTION (Client, Reliable)
	void ClientSelectSpell (Spells spell);

	UFUNCTION (Client, Reliable)
	void ClientRegisterGameStart ();

	TArray <Spells> _selectedSpells;

	bool _gameStarted = false;
};
