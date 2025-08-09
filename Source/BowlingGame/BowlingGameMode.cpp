// Copyright Epic Games, Inc. All Rights Reserved.

#include "BowlingGameMode.h"

#include "BowlingBall.h"
#include "BowlingGameUtils.h"
#include "BowlingPinSet.h"
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
}

void ABowlingGameGameMode::StartNewGame()
{
	Score = 0;
	CurrentFrame = 0;
	
	for (int32& FramePoints : Frames)
	{
		FramePoints = 0;
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
	CurrentFrame++;
	CurrentThrow = 1;
	ResetPinSet();
}

void ABowlingGameGameMode::ComputeScore()
{
	Score = 5; // TODO
}

void ABowlingGameGameMode::OnPinDown(AActor* Pin)
{
	UBowlingGameUtils::DebugPrint(TEXT("Pin down!"), 5.0f, FColor::Yellow);
	Frames[CurrentFrame - 1]++;
}

void ABowlingGameGameMode::OnBallThrown()
{
	bThrowInProgress = true;
	UBowlingGameUtils::DebugPrint(FString::Printf(TEXT("Ball thrown - Frame %d, Throw %d"), CurrentFrame, CurrentThrow), 5.0f, FColor::Red);
}

void ABowlingGameGameMode::OnBallPit(ABowlingGameProjectile* Ball)
{
	UBowlingGameUtils::DebugPrint(FString::Printf(TEXT("Ball pit - Frame %d, Throw %d"), CurrentFrame, CurrentThrow), 5.0f, FColor::Red);

	ComputeScore();

	bThrowInProgress = false;

	if (CurrentFrame < 21 && CurrentThrow == 2)
	{
		StartNewFrame();
	}
	else if (CurrentThrow == 3)
	{
		// Game ended
		bGameInProgress = false;
		OnGameEnded.Broadcast(Score);
	}
	else
	{
		CurrentThrow++;
	}

	Ball->Destroy();
}
