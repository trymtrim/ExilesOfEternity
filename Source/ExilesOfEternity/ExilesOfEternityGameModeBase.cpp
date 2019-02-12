// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameModeBase.h"
#include "ConstructorHelpers.h"
#include "PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "CharacterBase.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "PlayerStateBase.h"

AExilesOfEternityGameModeBase::AExilesOfEternityGameModeBase ()
{
	//Set default player controller class
	PlayerControllerClass = APlayerControllerBase::StaticClass ();

	//Set default player state class
	PlayerStateClass = APlayerStateBase::StaticClass ();

	//Set default pawn class
	static ConstructorHelpers::FClassFinder <APawn> PlayerPawnClass (TEXT ("/Game/Blueprints/Characters/Serath_BP"));

	if (PlayerPawnClass.Class != NULL)
		DefaultPawnClass = PlayerPawnClass.Class;
}

void AExilesOfEternityGameModeBase::BeginPlay ()
{
	//Check if there still are any players connected every few seconds
	FTimerHandle checkPlayerConnectionTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (checkPlayerConnectionTimerHandle, this, &AExilesOfEternityGameModeBase::CheckPlayerConnection, 5.0f, true);
}

FString AExilesOfEternityGameModeBase::InitNewPlayer (APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	//Set player name
	FString playerName = UGameplayStatics::ParseOption (Options, "PlayerName");

	if (playerName == "")
		playerName = "Player";

	Cast <APlayerStateBase> (NewPlayerController->PlayerState)->SetNickname (playerName);

	//Cast <APlayerControllerBase> (NewPlayerController)->SetPlayerName (playerName);

	return Super::InitNewPlayer (NewPlayerController, UniqueId, Options, Portal);
}

AActor* AExilesOfEternityGameModeBase::ChoosePlayerStart_Implementation (AController* Player)
{
	//For testing
	if (firstPlayerController == nullptr)
		firstPlayerController = Player;

	//Increase player count by one
	_playerCount++;

	//Set player team number
	Cast <APlayerStateBase> (Player->PlayerState)->SetTeamNumber (_playerCount);

	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Set player start location
	if (playerStarts.Num () >= _playerCount)
		return playerStarts [_playerCount - 1];

	return playerStarts [playerStarts.Num () - 1];
}

void AExilesOfEternityGameModeBase::ReportDeath (ACharacterBase* characterController)
{
	//Respawn character
	FTimerDelegate respawnDelegate = FTimerDelegate::CreateUObject (this, &AExilesOfEternityGameModeBase::RespawnCharacter, characterController);
	FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, respawnDelegate, 2.0f, false);
}

void AExilesOfEternityGameModeBase::RespawnCharacter (ACharacterBase* characterController)
{
	//Reset character
	characterController->ResetCharacter ();

	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Respawn character at new location
	int spawnIndex = FMath::RandRange (0, playerStarts.Num () - 1);
	characterController->SetActorLocation (playerStarts [spawnIndex]->GetActorLocation ());
}

void AExilesOfEternityGameModeBase::EndGame ()
{
	_gameEnded = true;
}

bool AExilesOfEternityGameModeBase::GetGameEnded ()
{
	return _gameEnded;
}

void AExilesOfEternityGameModeBase::CheckPlayerConnection ()
{
	//If there are no longer any players connected, exit game instance
	if (_playerCount > 0)
	{
		if (GetNumPlayers () == 0)
			GIsRequestingExit = true;
	}
}
