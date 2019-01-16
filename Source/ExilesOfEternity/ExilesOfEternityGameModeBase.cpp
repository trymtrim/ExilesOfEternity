// Copyright Sharp Raccoon 2019.

#include "ExilesOfEternityGameModeBase.h"
#include "ConstructorHelpers.h"

AExilesOfEternityGameModeBase::AExilesOfEternityGameModeBase ()
{
	//Set default pawn class
	static ConstructorHelpers::FClassFinder <APawn> PlayerPawnClass (TEXT ("/Game/Blueprints/Characters/Serath_BP"));

	if (PlayerPawnClass.Class != NULL)
		DefaultPawnClass = PlayerPawnClass.Class;
}
