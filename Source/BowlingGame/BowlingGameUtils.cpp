#include "BowlingGameUtils.h"
#include "Engine/Engine.h"

void UBowlingGameUtils::DebugPrint(const FString& Message, float TimeToDisplay, FColor Color)
{
	// Output log
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

	// On-screen debug
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, // Key (-1 means create a new message)
			TimeToDisplay, // Duration
			Color, // Color
			Message
		);
	}
}
