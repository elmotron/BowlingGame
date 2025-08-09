// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BowlingGameMode.generated.h"

class APinSet;
class ABowlingGameProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, int32, Score);

UCLASS(minimalapi)
class ABowlingGameGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABowlingGameGameMode();

	UPROPERTY(EditAnywhere, Category = "Bowling")
	TSubclassOf<APinSet> PinSetBlueprintClass;

	UPROPERTY()
	APinSet* SpawnedPinSet = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Bowling")
	int32 CurrentFrame = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Bowling")
	int32 CurrentThrow = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Bowling")
	int32 Score = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Bowling")
	bool bThrowInProgress = false;

	UPROPERTY(BlueprintReadOnly, Category = "Bowling")
	bool bGameInProgress = false;
	
	UFUNCTION(BlueprintCallable, Category = "Bowling")
	void StartNewGame();

	UPROPERTY(BlueprintAssignable, Category = "Bowling")
	FOnGameStarted OnGameStarted;

	UPROPERTY(BlueprintAssignable, Category = "Bowling")
	FOnGameEnded OnGameEnded;
	
	void OnBallThrown();

	// Called when the ball hits the pit area
	void OnBallPit(ABowlingGameProjectile* Ball);

	bool AllowThrow() const
	{
		return bGameInProgress && !bThrowInProgress;
	}

protected:
	virtual void BeginPlay() override;

	void ResetPinSet();
	void StartNewFrame();
	void ComputeScore();
	
	UFUNCTION()
	void OnPinDown(AActor* Pin);

private:
	TStaticArray<int32, 21> Frames;
};



