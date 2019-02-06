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

private:
	void CheckForGameStart ();
	void StartGame ();

	bool _gameStarted = false;
};
