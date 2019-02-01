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
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual FString InitNewPlayer (APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual AActor* ChoosePlayerStart_Implementation (AController* Player) override;

private:
	void RespawnCharacter (ACharacterBase* characterController);
	void CheckPlayerConnection ();

	int _playerCount = 0;
};
