// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ExilesOfEternityGameModeBase.generated.h"

class ACharacterBase;

UCLASS()
class EXILESOFETERNITY_API AExilesOfEternityGameModeBase : public AGameModeBase
{
	GENERATED_BODY ()

public:
	AExilesOfEternityGameModeBase ();

	//Called from character controller when the character dies
	void ReportDeath (ACharacterBase* characterController);

protected:
	virtual AActor* ChoosePlayerStart_Implementation (AController* Player) override;

private:
	void RespawnCharacter (ACharacterBase* characterController);

	int _playerCount = 0;
};
