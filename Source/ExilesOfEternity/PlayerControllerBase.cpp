// Copyright Sharp Raccoon 2019.

#include "PlayerControllerBase.h"
#include "Engine/World.h"

APlayerControllerBase::APlayerControllerBase ()
{

}

void APlayerControllerBase::BeginPlay ()
{
	//Disable mouse cursor
	ShowMouseCursor (false);
}

void APlayerControllerBase::SetTeamNumber (int number)
{
	_teamNumber = number;
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
