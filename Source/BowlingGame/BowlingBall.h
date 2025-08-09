// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BowlingBall.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ABowlingGameProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UStaticMeshComponent* CollisionComp;

public:
	void LaunchBall(const FVector& LaunchVelocity) const;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	bool bHitBallPit = false;
};
