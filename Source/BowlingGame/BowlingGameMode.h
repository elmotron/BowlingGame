// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BowlingCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameModeBase.h"
#include "BowlingGameMode.generated.h"

class APinSet;
class ABowlingGameProjectile;
class ABowlingGameCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, int32, Score);

UCLASS(minimalapi)
class ABowlingGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

	struct FBowlingFrame
	{
		int32 PinsDowned = 0;
		int32 CurrentThrow = 0;
	};

public:
	ABowlingGameGameMode();

	UPROPERTY(EditAnywhere, Category = "Bowling")
	TSubclassOf<APinSet> PinSetBlueprintClass;

	UPROPERTY()
	APinSet* SpawnedPinSet = nullptr;

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

	UFUNCTION(BlueprintCallable, Category = "Bowling")
	int32 GetCurrentFrame() const { return FMath::Min(10, CurrentThrowIndex / 2 + 1); }

	UFUNCTION(BlueprintCallable, Category = "Bowling")
	int32 GetCurrentThrow() const { return CurrentThrowIndex > 17 ? CurrentThrowIndex - 17 : (CurrentThrowIndex % 2) + 1; }

	void OnBallThrown();

	// Called when the ball hits the pit area
	void OnBallPit(ABowlingGameProjectile* Ball);

	UFUNCTION(BlueprintCallable, Category = "Bowling")
	bool AllowThrow() const
	{
		return bGameInProgress && !bThrowInProgress && IsPlayerFacingPins();
	}

protected:
	virtual void BeginPlay() override;

	void ResetPinSet();
	void StartNewFrame();
	void ComputeScore();
	void ProcessBallPit();

	bool IsStrike(const int ThrowIndex) const { return FrameThrows[ThrowIndex] == 10; }
	bool IsSpare(const int ThrowIndex) const { return (FrameThrows[ThrowIndex] + FrameThrows[ThrowIndex + 1]) == 10; }
	int StrikeBonus(const int ThrowIndex) const { return FrameThrows[ThrowIndex + 2] + FrameThrows[ThrowIndex + 3]; }
	int SpareBonus(const int ThrowIndex) const { return FrameThrows[ThrowIndex + 2]; }
	int SumOfPinsInFrame(const int ThrowIndex) const { return FrameThrows[ThrowIndex] + FrameThrows[ThrowIndex + 1]; }

	bool IsPlayerFacingPins() const
	{
		if (Character)
		{
			const float Cos40 = FMath::Cos(FMath::DegreesToRadians(30.0f));
			return FVector::DotProduct(
				Character->GetFirstPersonCameraComponent()->GetForwardVector(),
				FVector(1, 0, 0)) > Cos40;
		}
		return false;
	}
	
	UFUNCTION()
	void OnPinDown(AActor* Pin);

private:
	UPROPERTY() 
	ABowlingGameCharacter* Character = nullptr;

	// 9 frames * 2 throws + 3 throws for last frame
	TStaticArray<int32, 21> FrameThrows;
	int32 CurrentThrowIndex = 0;
};
