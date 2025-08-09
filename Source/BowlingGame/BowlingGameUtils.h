// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BowlingGameUtils.generated.h"

/**
 * 
 */
UCLASS()
class BOWLINGGAME_API UBowlingGameUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Prints a debug message on screen and in the output log */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void DebugPrint(const FString& Message, float TimeToDisplay = 2.0f, FColor Color = FColor::Green);
};
