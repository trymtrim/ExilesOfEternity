// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleGameState.h"
#include "BattleRoyalePlayerController.h"
#include "BattleRoyalePlayerState.h"
#include "Engine/World.h"
#include "SpellAttributes.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "CharacterBase.h"
#include "AICharacterBase.h"
#include "Kismet/GameplayStatics.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode ()
{
	//Set default player controller class
	PlayerControllerClass = ABattleRoyalePlayerController::StaticClass ();

	//Set default game state class
	GameStateClass = ABattleRoyaleGameState::StaticClass ();

	//Set default player state class
	PlayerStateClass = ABattleRoyalePlayerState::StaticClass ();

	//Enable tick
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleRoyaleGameMode::Tick (float DeltaTime)
{
	if (!_gameStarted)
		CheckForGameStart ();
}

void ABattleRoyaleGameMode::SetPlayerAmount (int playerAmount)
{
	_playerAmount = playerAmount;

	GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, FString::FromInt (_playerAmount));
}

void ABattleRoyaleGameMode::AddReadyPlayer ()
{
	_readyPlayerAmount++;
}

void ABattleRoyaleGameMode::PlaytestStartGame (AController* controller)
{
	if (_gameStarted || controller != firstPlayerController)
		return;

	_gameStarted = true;
	
	for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
	{
		ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

		if (!playerController->GetStartingZoneChosen ())
			playerController->AutoSelectStartingZone ();
	}

	FTimerHandle startGameTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (startGameTimerHandle, this, &ABattleRoyaleGameMode::StartGame, 1.0f, false);
}

void ABattleRoyaleGameMode::CheckForGameStart ()
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GameState);

	//If start timer is finished, start game
	if (gameState->GetGameStartTime () - gameState->GetServerWorldTimeSeconds () <= 0.0f || _readyPlayerAmount == _playerAmount && _playerAmount != 0)
	{
		//Automatically choose spawn location for every player that has not chosen yet
		for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
		{
			ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

			if (!playerController->GetStartingZoneChosen ())
				playerController->AutoSelectStartingZone ();

			//Register for player that the game is starting
			playerController->RegisterGameStarting ();
		}

		//Officially start game after one second
		FTimerHandle startGameTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startGameTimerHandle, this, &ABattleRoyaleGameMode::StartGame, 5.0f, false);

		//Set game to started
		_gameStarted = true;
	}
}

void ABattleRoyaleGameMode::StartGame ()
{
	//Register game start for game state
	Cast <ABattleRoyaleGameState> (GameState)->StartGame ();

	//Reguster gane start for all player controllers
	for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
	{
		ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

		playerController->RegisterGameStart ();
	}

	//Get all AIs in the map
	TArray <AActor*> AIs;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), AAICharacterBase::StaticClass (), AIs);

	//Register game start for all AIs
	for (int i = 0; i < AIs.Num (); i++)
		Cast <AAICharacterBase> (AIs [i])->RegisterGameStart ();
}

void ABattleRoyaleGameMode::ProcedurallySpawnSpellCapsules ()
{
	//Load game stage info from data asset
	FStringAssetReference GameStageAssetPath ("GameStageInfo'/Game/Miscellaneous/DataAssets/GameStageInfo_Data.GameStageInfo_Data'");
	UObject* gameStageInfo = GameStageAssetPath.TryLoad ();

	int spellCapsuleAmountForEachSpell = Cast <UGameStageInfo> (gameStageInfo)->AmountOfSpellCapsulesForEachSpell;

	for (int i = 1; i < USpellAttributes::GetSpellCount () + 1; i++)
	{
		for (int j = 0; j < spellCapsuleAmountForEachSpell; j++)
		{
			TSubclassOf <AActor> spawnableSpellCapsule = USpellAttributes::GetSpellCapsule (Spells (i));

			//Declare spawn parameters
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			FVector spawnPosition;
			FRotator spawnRotation = FRotator (0.0f, 0.0f, 0.0f);

			//Spawn spell capsule
			AActor* spellCapsule = GetWorld ()->SpawnActor <AActor> (spawnableSpellCapsule, spawnPosition, spawnRotation, spawnParams);

			//Set the spell capsule's location
			SetSpellCapsuleLocation (spellCapsule);
		}
	}
}

void ABattleRoyaleGameMode::SetSpellCapsuleLocation (AActor* spellCapsule)
{
	//Set random location
	FVector location = FVector (FMath::RandRange (0.0f, 100000.0f), FMath::RandRange (0.0f, 100000.0f), 50000.0f);
	spellCapsule->SetActorLocation (location);

	//Line trace down from capsules position
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position
	FVector start = spellCapsule->GetActorLocation ();
	FVector end = start - FVector (0.0f, 0.0f, 100000.0f);

	//If line trace hits anything, set location to what it hits
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_GameTraceChannel3, traceParams))
	{
		//If line trace hits a blocking volume, change to a new location
		if (hit.GetActor ()->ActorHasTag ("Block"))
			SetSpellCapsuleLocation (spellCapsule);
		else
			spellCapsule->SetActorLocation (hit.ImpactPoint);
	}
	else //If line trace doesn't hit anything, change to a new location
		SetSpellCapsuleLocation (spellCapsule);
}

void ABattleRoyaleGameMode::ReportDeath (ACharacterBase* characterController)
{
	//Get player state
	ABattleRoyalePlayerState* playerState = Cast <ABattleRoyalePlayerState> (characterController->GetPlayerState ());

	//If player is permanent dead or current redeem kill timer is less than respawn timer, return
	if (playerState->GetPermanentDead ())
		return;

	//Declare the time it takes before the player can respawn
	float timeBeforeRespawn = 1.5f;

	playerState->StartRespawnTimer (10.0f + timeBeforeRespawn);

	//Respawn character
	FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, characterController, &ACharacterBase::ClientHandleRespawn, timeBeforeRespawn, false);
}
