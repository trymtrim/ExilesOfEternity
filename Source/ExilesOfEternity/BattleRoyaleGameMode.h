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

	void SetPlayerAmount (int playerAmount);
	void AddReadyPlayer ();

	UFUNCTION (BlueprintCallable)
	void ProcedurallySpawnSpellCapsules ();

	virtual void ReportDeath (ACharacterBase* characterController) override;
	
	//For testing
	UFUNCTION (BlueprintCallable)
	void PlaytestStartGame (AController* controller);

private:
	void CheckForGameStart ();
	void StartGame ();
	void SetSpellCapsuleLocation (AActor* spellCapsule);

	int _playerAmount = 0;
	int _readyPlayerAmount = 0;
};
