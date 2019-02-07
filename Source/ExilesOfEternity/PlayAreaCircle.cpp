// Copyright Sharp Raccoon 2019.

#include "PlayAreaCircle.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "BattleRoyaleGameState.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlayerControllerBase.h"
#include "CharacterBase.h"

APlayAreaCircle::APlayAreaCircle ()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayAreaCircle::BeginPlay ()
{
	Super::BeginPlay ();

	if (GetWorld ()->IsServer ())
	{
		//Load game stage info from data asset
		UDataAsset* gameStageInfo = FindObject <UDataAsset> (ANY_PACKAGE, TEXT ("GameStageInfo'/Game/Miscellaneous/DataAssets/GameStageInfo_Data.GameStageInfo_Data'"));
		_gameStageInfo = Cast <UGameStageInfo> (gameStageInfo);

		//Set a random end position from list of end positions
		_endPosition = _gameStageInfo->StageEndPositions [FMath::RandRange (0, _gameStageInfo->StageEndPositions.Num () - 1)];

		//Check for players outside of play area once every second
		FTimerHandle checkPlayersOutsidePlayAreaTimerHandle;
		GetWorld ()->GetTimerManager ().SetTimer (checkPlayersOutsidePlayAreaTimerHandle, this, &APlayAreaCircle::CheckPlayersOutsidePlayArea, 1.0f, true);
	}
}

void APlayAreaCircle::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	//Update shrinking
	if (_currentlyShrinking)
		UpdateShrinking (DeltaTime);
}

void APlayAreaCircle::StartShrinking (int stage)
{
	//Set stage to the current game stage and start shrinking the circle
	_stage = stage;
	_currentlyShrinking = true;

	GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "Shrinking started - Stage: " + FString::FromInt (_stage));
}

void APlayAreaCircle::UpdateShrinking (float deltaTime)
{
	//Scale/move the circle until it has reached its end diameter/position based on current stage

	//Get current end diameter
	int currentEndDiameter = _gameStageInfo->StageDiameters [_stage - 1];
	int currentDiameter = GetActorScale ().X;

	//When the circle has reached the current end position, stop shrinking
	if (currentDiameter <= currentEndDiameter)
	{
		_currentlyShrinking = false;

		GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, "Shrinking stopped");
	}
}

void APlayAreaCircle::CheckPlayersOutsidePlayArea ()
{
	//Loop through all players
	for (FConstPlayerControllerIterator Iterator = GetWorld ()->GetPlayerControllerIterator (); Iterator; ++Iterator)
	{
		APlayerControllerBase* playerController = Cast <APlayerControllerBase> (*Iterator);

		if (playerController)
		{
			FVector playerLocation = playerController->GetCharacter ()->GetActorLocation ();

			//If the player is outside of the play area, deal damage
			if (FVector::Distance (FVector (GetActorLocation ().X, GetActorLocation ().Y, playerLocation.Z), playerLocation) > GetActorScale ().X * 50.0f)
			{
				float damage = 3.0f;
				UGameplayStatics::ApplyDamage (playerController->GetCharacter (), damage, nullptr, this, nullptr);
			}
		}
	}
}
