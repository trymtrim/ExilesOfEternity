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

void APlayAreaCircle::LoadGameStageInfo (UGameStageInfo* gameStageInfo)
{
	_gameStageInfo = gameStageInfo;

	if (GetWorld ()->IsServer ())
	{
		//Set start location
		_startLocation = GetActorLocation ();

		//Set a random end position from list of end positions
		_endLocation = _gameStageInfo->StageEndPositions [FMath::RandRange (0, _gameStageInfo->StageEndPositions.Num () - 1)];

		//Set scale
		int startDiameter = _gameStageInfo->StageDiameters [0];
		SetActorScale3D (FVector (startDiameter, startDiameter, startDiameter));

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
	//Update total move time
	_totalMoveTime += deltaTime;

	//Get current end diameter
	int currentEndDiameter = _gameStageInfo->StageDiameters [_stage - 1];

	//Set shrink speed
	float shrinkSpeed = _gameStageInfo->ShrinkSpeed;

	//Scale circle
	FVector deltaScale = FVector (shrinkSpeed * deltaTime);
	deltaScale.Z = 0.0f;

	SetActorScale3D (GetActorScale3D () - deltaScale);

	//Move circle
	int startDiameter = _gameStageInfo->StageDiameters [0];
	int endDiameter = _gameStageInfo->StageDiameters [_gameStageInfo->StageDiameters.Num () - 1];
	int difference = startDiameter - endDiameter;

	float moveTime = (float) difference / shrinkSpeed;
	float moveAlpha = _totalMoveTime / moveTime;

	SetActorLocation (FMath::Lerp (_startLocation, FVector (_endLocation.X, _endLocation.Y, _startLocation.Z), moveAlpha));

	//Get current diameter
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
			//Get current player location
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
