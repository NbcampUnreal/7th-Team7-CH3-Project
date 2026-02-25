// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboCharacter.h"
#include "Character/KirboStatComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Team7_CH3_Project/UI/DamageFloatingText.h" // KH 260224 추가 : 플로팅 텍스트

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
    SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	StatComp = CreateDefaultSubobject<UKirboStatComponent>(TEXT("StatComp"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	StaminaPlaneComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaminaPlane"));
	StaminaPlaneComp->SetupAttachment(RootComponent);
	StaminaPlaneComp->SetUsingAbsoluteRotation(true);
	StaminaPlaneComp->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	StaminaPlaneComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
}

void AKirboCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StaminaPlaneComp)
	{
		StaminaMaterialDynamic = StaminaPlaneComp->CreateAndSetMaterialInstanceDynamic(0);

		if (StaminaMaterialDynamic)
		{
			StaminaMaterialDynamic->SetScalarParameterValue(FName("Percent"), 1.0f);
		}
	}

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

        // StatComp가 제대로 생성되었는지, StatData가 존재하는지 확인
        if (StatComp && StatData)
        {
            StatComp->InitializeStats(*StatData);\
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("StatComp or StatData is NULL at BeginPlay!"));
        }
    }
}

void AKirboCharacter::EnablePlayerInput()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
		bIsControlEnabled = true;
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
		if (StatComp)
		{
			float RecoveryRate = StatComp->GetStaminaRecoveryRate();
			StatComp->RecoverStamina(RecoveryRate * DeltaTime);
		}
	}

	if (StaminaPlaneComp)
	{
		FVector CharLoc = GetActorLocation();
		StaminaPlaneComp->SetWorldLocation(FVector(CharLoc.X, CharLoc.Y, CharLoc.Z - 80.0f)); // 높이 미세 조정
	}

	if (StatComp)
	{
		UpdateStamina(StatComp->CurrentStamina, StatComp->BaseStat.MaxStamina);
	}

	if (bIsControlEnabled)
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
    // KH 260224 추가 : 플로팅 텍스트 테스트용 K키
    PlayerInputComponent->BindKey(EKeys::K, IE_Pressed, this, &AKirboCharacter::TestSelfDamage);

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
		FVector InputWorldDir = FVector(MovementVector.X, MovementVector.Y, 0.f).GetSafeNormal();
		FVector CharacterForward = GetActorForwardVector();
		float DotResult = FVector::DotProduct(InputWorldDir, CharacterForward);
		float SpeedMultiplier = 1.0f;

		if (DotResult < -0.1f)
		{
			SpeedMultiplier = 0.6f;
		}

		AddMovementInput(FVector::ForwardVector, MovementVector.X * SpeedMultiplier);
		AddMovementInput(FVector::RightVector, MovementVector.Y * SpeedMultiplier);
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
	if (!StatComp) return;
	float DashCost = StatComp->GetDashStaminaCost();

	if (!bCanDash || (StatComp && StatComp->CurrentStamina < DashCost)) return;

	bIsInvincible = true;

	FVector DashDir = GetVelocity().IsNearlyZero() ? GetActorForwardVector() : GetVelocity().GetSafeNormal();
	LaunchCharacter(DashDir * 3000.f, true, true);

	StatComp->UseStamina(DashCost);

	bCanDash = false;
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &AKirboCharacter::ResetDash, 0.5f, false);
}

void AKirboCharacter::ResetDash()
{
	bCanDash = true;
	bIsInvincible = false;
}

void AKirboCharacter::ResetInvincibility()
{
	bIsInvincible = false;
}

void AKirboCharacter::HandleDeath()
{
	if (bIsDead) return;
	bIsDead = true;
	bIsControlEnabled = false;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
	{
		UISub->TriggerGameResult(false);
	}
}

// KH 260224 추가 : 플로팅 텍스트 테스트용
void AKirboCharacter::TestSelfDamage()
{
    UGameplayStatics::ApplyDamage(this, 10.0f, GetController(), this, UDamageType::StaticClass());
}
void AKirboCharacter::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	if (StaminaMaterialDynamic)
	{
		if (MaxStamina <= 0.0f) MaxStamina = 1.0f;
		float Alpha = FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f);
		StaminaMaterialDynamic->SetScalarParameterValue(FName("Percent"), Alpha);
	}
}

float AKirboCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead || bIsInvincible || ActualDamage <= 0.f || !StatComp)
	{
		return 0.f;
	}

	StatComp->TakeDamage(ActualDamage);

	if (StatComp->CurrentHP > 0.f)
	{
		if (HitMontage)
		{
			PlayAnimMontage(HitMontage);
		}

		bIsInvincible = true;
        // KH 260224 추가 : 여기서 받는 데미지 딜레이 속도 조절 가능
		GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &AKirboCharacter::ResetInvincibility, 0.3f, false);
	}
	else
	{
		HandleDeath();
	}

	return ActualDamage;
}
