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

	//Set default pawn classes
	static ConstructorHelpers::FClassFinder <APawn> serathPawnClass (TEXT ("/Game/Blueprints/Characters/Serath_BP"));
	if (serathPawnClass.Class != NULL)
		_serathCharacter = serathPawnClass.Class;

	static ConstructorHelpers::FClassFinder <APawn> gideonPawnClass (TEXT ("/Game/Blueprints/Characters/Gideon_BP"));
	if (gideonPawnClass.Class != NULL)
		_gideonCharacter = gideonPawnClass.Class;
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

	//Declare spawn parameters
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	FVector spawnPosition = FVector (0.0f, 0.0f, 100.0f);
	FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);

	TSubclassOf <APawn> characterClass = nullptr;
	FString characterName = UGameplayStatics::ParseOption (Options, "CharacterName");

	if (characterName == "Serath")
		characterClass = _serathCharacter;
	else if (characterName == "Gideon")
		characterClass = _gideonCharacter;
	else
		characterClass = _serathCharacter;

	//Spawn spell capsule
	APawn* character = GetWorld ()->SpawnActor <APawn> (characterClass, spawnPosition, spawnRotation, spawnParams);
	NewPlayerController->Possess (character);

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
	if (UGameplayStatics::GetCurrentLevelName (GetWorld ()) != "ArenaLevel")
		Cast <APlayerStateBase> (Player->PlayerState)->SetTeamNumber (_playerCount);

	//Get all start locations
	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	//Set player start location
	if (playerStarts.Num () >= _playerCount)
	{
		Player->GetPawn ()->SetActorLocation (playerStarts [_playerCount - 1]->GetActorLocation ());
		return playerStarts [_playerCount - 1];
	}

	Player->GetPawn ()->SetActorLocation (playerStarts [playerStarts.Num () - 1]->GetActorLocation ());
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
	//If there no longer are any players connected, exit game instance
	if (_playerCount > 0)
	{
		if (GetNumPlayers () == 0)
			GIsRequestingExit = true;
	}
	else
	{
		//If no players have connected within 30 seconds, exit game instance
		_playerConnectionCheckCount++;

		if (_playerConnectionCheckCount == 6)
			GIsRequestingExit = true;
	}
}
