#include "BowlingPinSet.h"

APinSet::APinSet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APinSet::BeginPlay()
{
	Super::BeginPlay();
}

void APinSet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

