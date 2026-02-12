// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboCharacter.h"
#include "Character/KirboStatComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h" // UI 연결

AKirboCharacter::AKirboCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 1000.f;
	SpringArmComp->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));

	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritYaw = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	StatComp = CreateDefaultSubobject<UKirboStatComponent>(TEXT("StatComp"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AKirboCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultContext, 0);
		}

		DisableInput(PlayerController);
	}

	FTimerHandle InputTimerHandle;
	GetWorldTimerManager().SetTimer(InputTimerHandle, this, &AKirboCharacter::EnablePlayerInput, 5.0f, false);

	if (CharacterStatTable)
	{
		static const FString ContextString(TEXT("Stat Init"));
		FKirboStatRow* StatData = CharacterStatTable->FindRow<FKirboStatRow>(FName("Player"), ContextString);

		if (StatData)
		{
			StatComp->InitializeStats(*StatData);
			GetCharacterMovement()->MaxWalkSpeed = StatComp->GetMoveSpeed();
			DashCooldownTime = StatComp->GetDodgeCooldown();
		}
	}

    // UI
    if (CharacterStatTable)
    {
        static const FString ContextString(TEXT("Stat Init"));
        FKirboStatRow* StatData = CharacterStatTable->FindRow<FKirboStatRow>(FName("Player"), ContextString);

        if (StatData)
        {
            StatComp->InitializeStats(*StatData);
            GetCharacterMovement()->MaxWalkSpeed = StatComp->GetMoveSpeed();
            DashCooldownTime = StatComp->GetDodgeCooldown();

            if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
            {
                UISub->BroadcastHPUpdate(StatComp->CurrentHP, StatComp->BaseStat.MaxHP);
            }
        }
    }
}

void AKirboCharacter::EnablePlayerInput()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}
}

void AKirboCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float SprintSpeed = StatComp->GetSprintSpeed();

	if (GetCharacterMovement()->MaxWalkSpeed >= SprintSpeed - 1.0f)
	{
		float Cost = StatComp->BaseStat.SprintStaminaCost * DeltaTime;
		if (!StatComp->UseStamina(Cost))
		{
			StopSprint();
		}
	}
	else
	{
		StatComp->RecoverStamina(5.0f * DeltaTime);
	}

	if (bIsShooting)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		FHitResult Hit;
		if (PlayerController && PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			FVector LookDir = Hit.ImpactPoint - GetActorLocation();
			LookDir.Z = 0.f;
			SetActorRotation(FRotationMatrix::MakeFromX(LookDir).Rotator());
		}
	}
}

void AKirboCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AKirboCharacter::Move
		);
		EnhancedInputComponent->BindAction(
			ShotAction,
			ETriggerEvent::Started,
			this,
			&AKirboCharacter::StartShot
		);
		EnhancedInputComponent->BindAction(
			ShotAction,
			ETriggerEvent::Completed,
			this,
			&AKirboCharacter::StopShot
		);
		EnhancedInputComponent->BindAction(
			SprintAction,
			ETriggerEvent::Started,
			this,
			&AKirboCharacter::StartSprint
		);
		EnhancedInputComponent->BindAction(
			SprintAction,
			ETriggerEvent::Completed,
			this,
			&AKirboCharacter::StopSprint
		);
		EnhancedInputComponent->BindAction(
			DashAction,
			ETriggerEvent::Started,
			this,
			&AKirboCharacter::Dash
		);
	}


}

void AKirboCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(FVector::ForwardVector, MovementVector.X);
		AddMovementInput(FVector::RightVector, MovementVector.Y);
	}
}

void AKirboCharacter::StartShot()
{
	bIsShooting = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AKirboCharacter::StopShot()
{
	bIsShooting = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AKirboCharacter::StartSprint()
{
	if (StatComp->CurrentStamina > 0)
	{
		GetCharacterMovement()->MaxWalkSpeed = StatComp->GetSprintSpeed();
	}
}

void AKirboCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = StatComp->GetMoveSpeed();
}

void AKirboCharacter::Dash()
{
	if (!bCanDash) return;

	FVector DashDir = GetVelocity().IsNearlyZero() ? GetActorForwardVector() : GetVelocity().GetSafeNormal();
	LaunchCharacter(DashDir * 3000.f, true, true);

	bCanDash = false;
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &AKirboCharacter::ResetDash, DashCooldownTime, false);
}

void AKirboCharacter::ResetDash()
{
	bCanDash = true;
}

