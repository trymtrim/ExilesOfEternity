// Copyright Sharp Raccoon 2019.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleGameState.h"
#include "BattleRoyalePlayerController.h"
#include "BattleRoyalePlayerState.h"
#include "Engine/World.h"
#include "SpellAttributes.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "CharacterBase.h"

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

void ABattleRoyaleGameMode::CheckForGameStart ()
{
	//Get game state
	ABattleRoyaleGameState* gameState = Cast <ABattleRoyaleGameState> (GameState);

	//If start timer is finished, start game
	if (gameState->GetGameStartTime () - gameState->GetServerWorldTimeSeconds () <= 0.0f)
	{
		//Automatically choose spawn location for every player that has not chosen yet
		for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
		{
			ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

			if (!playerController->GetStartingZoneChosen ())
				playerController->AutoSelectStartingZone ();
		}

		//Officially start game after one second
		FTimerHandle startGameTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (startGameTimerHandle, this, &ABattleRoyaleGameMode::StartGame, 1.0f, false);

		//Set game to started
		_gameStarted = true;
	}
}

void ABattleRoyaleGameMode::StartGame ()
{
	//Update game state
	Cast <ABattleRoyaleGameState> (GameState)->StartGame ();

	//Update player controllers
	for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
	{
		ABattleRoyalePlayerController* playerController = Cast <ABattleRoyalePlayerController> (Cast <APlayerController> (*Iterator));

		playerController->RegisterGameStart ();
	}
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
	//Declare respawn time
	float respawnTime = 2.0f;

	//Get player state
	ABattleRoyalePlayerState* playerState = Cast <ABattleRoyalePlayerState> (characterController->GetPlayerState ());

	//If player is permanent dead or current redeem kill timer is less than respawn timer, return
	if (playerState->GetPermanentDead () || playerState->GetCurrentRedeemKillTime () <= respawnTime)
		return;

	//Respawn character
	FTimerDelegate respawnDelegate = FTimerDelegate::CreateUObject (this, &ABattleRoyaleGameMode::RespawnCharacter, characterController);
	FTimerHandle respawnTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (respawnTimerHandle, respawnDelegate, respawnTime, false);
}
