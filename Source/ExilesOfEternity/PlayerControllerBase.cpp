// Copyright Sharp Raccoon 2019.

#include "PlayerControllerBase.h"
#include "Engine/World.h"
#include "CharacterBase.h"
#include "ExilesOfEternityCheatManager.h"
#include "ConstructorHelpers.h"

APlayerControllerBase::APlayerControllerBase ()
{
	//Set cheat manager class
	CheatClass = UExilesOfEternityCheatManager::StaticClass ();

	bEnableMouseOverEvents = true;
}

void APlayerControllerBase::BeginPlay ()
{
	//Disable mouse cursor
	ShowMouseCursor (false);
}

void APlayerControllerBase::SetInputUIOnly_Implementation ()
{
	//Enable mouse cursor
	bShowMouseCursor = true;

	//Set input mode to UI
	FInputModeUIOnly uiInputMode;
	SetInputMode (uiInputMode);
}

void APlayerControllerBase::AddMessage_Implementation (const FString& message, bool error)
{
	OnMessageBP.Broadcast (message, error);
}

void APlayerControllerBase::AddBigMessage_Implementation (const FString& message, float duration = 0.0f)
{
	OnBigMessageBP.Broadcast (message, duration);
}

void APlayerControllerBase::AddKillMessage_Implementation (const FString& message)
{
	OnKillMessageBP.Broadcast (message);
}

void APlayerControllerBase::Possess (APawn* InPawn)
{
	Super::Possess (InPawn);

	/*//Get the character controller
	ACharacterBase* character = Cast <ACharacterBase> (InPawn);

	//Initialize character
	character->InitializeCharacter ();*/
}

void APlayerControllerBase::ShowMouseCursor_Implementation (bool state)
{
	if (gameFinished)
		return;

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

		if (GetCharacter () != nullptr)
			Cast <ACharacterBase> (GetCharacter ())->SetMovingSpell (false);
	}

	//Set mouse position to bottom center of screen
	int viewPortX;
	int viewPortY;

	GetViewportSize (viewPortX, viewPortY);
	SetMouseLocation (viewPortX / 2, viewPortY - (viewPortY / 8));
}

void APlayerControllerBase::SetupInputComponent ()
{
	Super::SetupInputComponent ();

	check (InputComponent);

	//Set up action binding
	InputComponent->BindAction ("ShowMouseCursor", IE_Pressed, this, &APlayerControllerBase::ShowMouseCursor <true>);
	InputComponent->BindAction ("ShowMouseCursor", IE_Released, this, &APlayerControllerBase::ShowMouseCursor <false>);
}
