// Copyright Sharp Raccoon 2019.

#include "PlayerStateBase.h"
#include "UnrealNetwork.h"

APlayerStateBase::APlayerStateBase ()
{
	bUseCustomPlayerNames = true;
}

void APlayerStateBase::SetNickname (FString name)
{
	_nickname = name;
}

void APlayerStateBase::SetTeamNumber (int number)
{
	_teamNumber = number;
}

void APlayerStateBase::AddKill ()
{
	_kills++;
}

void APlayerStateBase::AddDeath ()
{
	_deaths++;
}

FString APlayerStateBase::GetNickname ()
{
	return _nickname;
}

int APlayerStateBase::GetTeamNumber ()
{
	return _teamNumber;
}

int APlayerStateBase::GetKills ()
{
	return _kills;
}

int APlayerStateBase::GetDeaths ()
{
	return _deaths;
}

void APlayerStateBase::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (APlayerStateBase, _nickname);
	DOREPLIFETIME (APlayerStateBase, _kills);
	DOREPLIFETIME (APlayerStateBase, _deaths);
}
