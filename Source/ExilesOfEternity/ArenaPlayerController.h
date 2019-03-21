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

	UFUNCTION (BlueprintCallable)
	int GetSelectedSpells ();

protected:
	virtual void BeginPlay () override;

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void SetTeamNumber ();

	UPROPERTY (Replicated)
	int _selectedSpells = 0;
};
