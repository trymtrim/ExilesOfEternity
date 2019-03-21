// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "ExilesOfEternityGameModeBase.h"
#include "ArenaGameMode.generated.h"

UCLASS()
class EXILESOFETERNITY_API AArenaGameMode : public AExilesOfEternityGameModeBase
{
	GENERATED_BODY()
	
public:
	AArenaGameMode ();

protected:
	//virtual void BeginPlay () override;
	//virtual void ReportDeath (ACharacterBase* characterController) override;

private:
	void StartGame ();
};
