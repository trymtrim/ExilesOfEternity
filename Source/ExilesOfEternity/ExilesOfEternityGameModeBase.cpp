// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameModeBase.h"
#include "ConstructorHelpers.h"
#include "PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "CharacterBase.h"
#include "Runtime/Engine/Public/TimerManager.h"

AExilesOfEternityGameModeBase::AExilesOfEternityGameModeBase ()
{
	//Set default player controller class
	PlayerControllerClass = APlayerControllerBase::StaticClass ();

	//Set default pawn class
	static ConstructorHelpers::FClassFinder <APawn> PlayerPawnClass (TEXT ("/Game/Blueprints/Characters/Serath_BP"));

	if (PlayerPawnClass.Class != NULL)
		DefaultPawnClass = PlayerPawnClass.Class;
}

void AExilesOfEternityGameModeBase::BeginPlay ()
{

}

AActor* AExilesOfEternityGameModeBase::ChoosePlayerStart_Implementation (AController* Player)
{
	//Increase player count by one
	_playerCount++;

	//Set player team number
	Cast <APlayerControllerBase> (Player)->SetTeamNumber (_playerCount);

	//Set player start location
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	if (playerStarts.Num () >= _playerCount)
		return playerStarts [_playerCount - 1];

	return playerStarts [playerStarts.Num () - 1];
}

void AExilesOfEternityGameModeBase::ReportDeath (ACharacterBase* characterController)
{
	//Reset character
	FTimerDelegate respawnDelegate = FTimerDelegate::CreateUObject (this, &AExilesOfEternityGameModeBase::RespawnCharacter, characterController);
	FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, respawnDelegate, 2.0f, false);
}

void AExilesOfEternityGameModeBase::RespawnCharacter (ACharacterBase* characterController)
{
	//Reset character
	characterController->ResetCharacter ();

	//Respawn character at new location
	characterController->SetActorLocation (ChoosePlayerStart (characterController->GetController ())->GetActorLocation ());
}
