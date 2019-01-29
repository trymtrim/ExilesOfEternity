// Copyright Sharp Raccoon 2019.

#include "PlayerControllerBase.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "CharacterBase.h"
#include "ExilesOfEternityCheatManager.h"
#include "ConstructorHelpers.h"

APlayerControllerBase::APlayerControllerBase ()
{
	//Set cheat manager class
	CheatClass = UExilesOfEternityCheatManager::StaticClass ();
}

void APlayerControllerBase::BeginPlay ()
{
	//Disable mouse cursor
	ShowMouseCursor (false);
}

void APlayerControllerBase::Possess (APawn* InPawn)
{
	Super::Possess (InPawn);

	//Get the character controller
	ACharacterBase* character = Cast <ACharacterBase> (InPawn);

	//Initialize character
	character->InitializeCharacter (_playerName);
}

void APlayerControllerBase::SetTeamNumber (int number)
{
	_teamNumber = number;
}

void APlayerControllerBase::SetPlayerName (FString playerName)
{
	_playerName = playerName;
}

int APlayerControllerBase::GetTeamNumber ()
{
	return _teamNumber;
}

void APlayerControllerBase::ShowMouseCursor (bool state)
{
	//Enable or disable mouse cursor
	bShowMouseCursor = state;

	if (state)
	{
		//Set input mode to UI
		FInputModeGameAndUI uiInputMode;
		SetInputMode (uiInputMode);
	}
	else
	{
		//Set input mode to game
		FInputModeGameOnly gameInputMode;
		SetInputMode (gameInputMode);
	}

	//Set mouse position to center bottom of screen
	int viewPortX;
	int viewPortY;

	GetViewportSize (viewPortX, viewPortY);
	GetWorld ()->GetFirstPlayerController ()->SetMouseLocation (viewPortX / 2, viewPortY - (viewPortY / 8));
}

void APlayerControllerBase::SetupInputComponent ()
{
	Super::SetupInputComponent ();

	check (InputComponent);

	//Set up action binding
	InputComponent->BindAction ("ShowMouseCursor", IE_Pressed, this, &APlayerControllerBase::ShowMouseCursor <true>);
	InputComponent->BindAction ("ShowMouseCursor", IE_Released, this, &APlayerControllerBase::ShowMouseCursor <false>);
}
