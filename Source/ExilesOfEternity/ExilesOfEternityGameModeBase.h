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
	virtual void ReportDeath (ACharacterBase* characterController);
	
	void EndGame ();

	bool GetGameEnded ();

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	virtual FString InitNewPlayer (APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual AActor* ChoosePlayerStart_Implementation (AController* Player) override;

	void RespawnCharacter (ACharacterBase* characterController);

private:
	void CheckPlayerConnection ();

	int _playerCount = 0;

	bool _gameEnded = false;
};
