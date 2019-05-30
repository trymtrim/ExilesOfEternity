// Copyright Sharp Raccoon 2019.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams (FOnMessage, FString, message, bool, error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams (FOnBigMessage, FString, message, float, duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnKillMessage, FString, message);

UCLASS()
class EXILESOFETERNITY_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	APlayerControllerBase ();

	//Called to bind functionality to input
	virtual void SetupInputComponent () override;

	UFUNCTION (Client, Reliable, BlueprintCallable)
	void SetInputUIOnly ();

	UFUNCTION (Client, Reliable, BlueprintCallable)
	void AddMessage (const FString& message, bool error);
	UFUNCTION (Client, Reliable)
	void AddBigMessage (const FString& message, float duration = 0.0f);
	UFUNCTION (Client, Reliable)
	void AddKillMessage (const FString& message);

	UPROPERTY (BlueprintAssignable)
	FOnMessage OnMessageBP;
	UPROPERTY (BlueprintAssignable)
	FOnBigMessage OnBigMessageBP;
	UPROPERTY (BlueprintAssignable)
	FOnKillMessage OnKillMessageBP;

	UPROPERTY (BlueprintReadWrite)
	bool gameFinished = false;

	UFUNCTION (Client, Reliable, BlueprintCallable)
	void ShowMouseCursor (bool state);

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;
	virtual void Possess (APawn* InPawn) override;

private:
	template <bool state>
	void ShowMouseCursor ()
	{
		ShowMouseCursor (state);
	}
};
