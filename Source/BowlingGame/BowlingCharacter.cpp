// Copyright Epic Games, Inc. All Rights Reserved.

#include "BowlingCharacter.h"

#include "BowlingGameMode.h"
#include "BowlingBall.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ABowlingGameCharacter::ABowlingGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void ABowlingGameCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABowlingGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// Jumping
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

			// Moving
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABowlingGameCharacter::Move);

			// Looking
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABowlingGameCharacter::Look);

			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABowlingGameCharacter::ThrowBowlingBall);		
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
		}
	}
}

void ABowlingGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABowlingGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABowlingGameCharacter::ThrowBowlingBall()
{
	if (!BallClass)
	{
		return;
	}

	// Disallow throw when the ball is still rolling
	ABowlingGameGameMode* GameMode = Cast<ABowlingGameGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode || !GameMode->AllowThrow())
	{
		return;
	}

	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	// Start location of ball (in front of camera)
	const FVector SpawnLocation = (FirstPersonCameraComponent->GetComponentLocation() - FirstPersonCameraComponent->GetUpVector() * 80.0f) + FirstPersonCameraComponent->GetForwardVector() * 150.f;
	const FRotator SpawnRotation = FirstPersonCameraComponent->GetComponentRotation();

	// Spawn the ball
	const ABowlingGameProjectile* Ball = GetWorld()->SpawnActor<ABowlingGameProjectile>(
		BallClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Ball)
	{
		// Direction from camera forward vector
		const FVector LaunchDirection = FirstPersonCameraComponent->GetForwardVector();

		// Scale by desired throw force
		constexpr float ThrowStrength = 1000.f;
		Ball->LaunchBall(LaunchDirection * ThrowStrength);
		GameMode->OnBallThrown();
	}
}
