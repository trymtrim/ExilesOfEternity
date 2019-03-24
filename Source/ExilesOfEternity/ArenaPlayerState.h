// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStateBase.h"
#include "ArenaPlayerState.generated.h"

UCLASS()
class EXILESOFETERNITY_API AArenaPlayerState : public APlayerStateBase
{
	GENERATED_BODY ()

public:
	virtual void Destroyed () override;

protected:
	virtual void OnKill (APlayerState* playerState) override;
};
