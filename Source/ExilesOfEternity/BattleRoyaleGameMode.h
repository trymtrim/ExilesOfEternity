// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "ExilesOfEternityGameModeBase.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS()
class EXILESOFETERNITY_API ABattleRoyaleGameMode : public AExilesOfEternityGameModeBase
{
	GENERATED_BODY ()
	
public:
	ABattleRoyaleGameMode ();

	virtual void Tick (float DeltaTime) override;

	UFUNCTION (BlueprintCallable)
	void ProcedurallySpawnSpellCapsules ();

private:
	void CheckForGameStart ();
	void StartGame ();
	void SetSpellCapsuleLocation (AActor* spellCapsule);

	bool _gameStarted = false;
};
