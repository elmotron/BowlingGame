// Copyright Epic Games, Inc. All Rights Reserved.

#include "BowlingGameMode.h"

#include "BowlingBall.h"
#include "BowlingCharacter.h"
#include "BowlingGameUtils.h"
#include "BowlingPinSet.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABowlingGameGameMode::ABowlingGameGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void ABowlingGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		Character = Cast<ABowlingGameCharacter>(Pawn);
	}	
}

void ABowlingGameGameMode::StartNewGame()
{
	Score = 0;
	CurrentThrowIndex = -1;
	
	for (auto& FrameThrow : FrameThrows)
	{
		FrameThrow = 0;
	}	

	StartNewFrame();
	
	bGameInProgress = true;

	OnGameStarted.Broadcast();
}

void ABowlingGameGameMode::ResetPinSet()
{
	if (!PinSetBlueprintClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PinSetBlueprintClass is not set in GameMode"));
		return;
	}

	if (SpawnedPinSet)
	{
		SpawnedPinSet->Destroy();
	}

	const FVector SpawnLocation(2650.0f, 260.0f, 0.f);
	const FRotator SpawnRotation = FRotator::MakeFromEuler({0.0f, 0.0f, 30.0f});

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedPinSet = GetWorld()->SpawnActor<APinSet>(
		PinSetBlueprintClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	SpawnedPinSet->OnPinDown.AddDynamic(this, &ABowlingGameGameMode::OnPinDown);
}

void ABowlingGameGameMode::StartNewFrame()
{
	if (CurrentThrowIndex >= 0)
	{
		// Advance to next frame
		CurrentThrowIndex += 2 - (CurrentThrowIndex % 2);	
	}
	else
	{
		CurrentThrowIndex = 0;
	}

	ResetPinSet();
}

void ABowlingGameGameMode::ComputeScore()
{
	Score = 0;
	int ThrowIndex = 0;
	for (int Frame = 0; Frame < 10; ++Frame, ThrowIndex += 2)
	{
		if (IsStrike(ThrowIndex))
		{
			Score += 10 + StrikeBonus(ThrowIndex);
		}
		else if (IsSpare(ThrowIndex))
		{
			Score += 10 + SpareBonus(ThrowIndex);
		}
		else
		{
			// Open frame
			Score += SumOfPinsInFrame(ThrowIndex);
		}
	}
}

void ABowlingGameGameMode::ProcessBallPit()
{
	bThrowInProgress = false;

	const int32 CurrentThrow = GetCurrentThrow();
	const int32 CurrentFrame = GetCurrentFrame();

	const int32 CurrentThrowPinsDown = FrameThrows[CurrentThrowIndex];
	const int32 CurrentFramePinsDown = FrameThrows[CurrentThrowIndex] + (CurrentThrow > 1 ? FrameThrows[CurrentThrowIndex - 1] : 0);

	if (CurrentFrame < 10 && (CurrentThrow == 2 || CurrentThrowPinsDown == 10))
	{
		StartNewFrame();
	}
	else if (CurrentFrame == 10)
	{
		// Allow extra throws in last frame on spare or strike
		if (CurrentThrow == 3 || (CurrentThrow == 2 && CurrentFramePinsDown < 10))
		{
			// Game ended
			ComputeScore();
			bGameInProgress = false;
			OnGameEnded.Broadcast(Score);
		}
		else
		{
			if (CurrentFramePinsDown % 10 == 0)
			{
				ResetPinSet();	
			}
			CurrentThrowIndex++;	
		}
	}
	else
	{
		CurrentThrowIndex++;
	}
}

void ABowlingGameGameMode::OnPinDown(AActor* Pin)
{
	UBowlingGameUtils::DebugPrint(TEXT("Pin down!"), 5.0f, FColor::Yellow);
	FrameThrows[CurrentThrowIndex]++;
}

void ABowlingGameGameMode::OnBallThrown()
{
	bThrowInProgress = true;
	UBowlingGameUtils::DebugPrint(FString::Printf(TEXT("Ball thrown - Frame %d, Throw %d"), GetCurrentFrame(), GetCurrentThrow()), 5.0f, FColor::Red);
}

void ABowlingGameGameMode::OnBallPit(ABowlingGameProjectile* Ball)
{
	Ball->Destroy();

	// Give the pins some time to settle

	FTimerHandle* NewTimerHandle = new FTimerHandle();

	GetWorldTimerManager().SetTimer(
		*NewTimerHandle,
		[this, NewTimerHandle]()
		{
			ProcessBallPit();
			delete NewTimerHandle;
		},
		2.0,
		false
	);	
}
