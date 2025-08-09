// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BowlingPinSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinDown, AActor*, PinActor);

UCLASS()
class BOWLINGGAME_API APinSet : public AActor
{
	GENERATED_BODY()
	
public:	
	APinSet();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPinDown OnPinDown;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TriggerOnPinDown(AActor* PinActor) const
	{
		OnPinDown.Broadcast(PinActor);
	}
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
