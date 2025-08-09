#include "BowlingBall.h"

#include "BowlingGameMode.h"
#include "BowlingGameUtils.h"

void ABowlingGameProjectile::LaunchBall(const FVector& LaunchVelocity) const
{
	if (CollisionComp && CollisionComp->IsSimulatingPhysics())
	{
		CollisionComp->AddImpulse(LaunchVelocity, NAME_None, true);
	}
}

void ABowlingGameProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ABowlingGameProjectile::OnHit);	
	}
}

void ABowlingGameProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bHitBallPit && OtherActor != nullptr && OtherActor->ActorHasTag("BallPit"))
	{
		UBowlingGameUtils::DebugPrint(TEXT("Hit BallPit"));
		bHitBallPit = true;
		if (ABowlingGameGameMode* GameMode = Cast<ABowlingGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->OnBallPit(this);
		}
	}
}
