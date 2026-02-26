// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyData.h"
#include "Enemy/EnemyAIControl.h"
#include "Enemy/EnemyProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/KirboGameMode.h"
#include "Team7_CH3_Project/Public/Character/KirboStatComponent.h"
#include "Team7_CH3_Project/UI/EnemyHealthBarComponent.h"
#include "Team7_CH3_Project/UI/DamageFloatingText.h"
#include "Team7_CH3_Project/UI/DevHHUD.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	HealthBarComp = CreateDefaultSubobject<UEnemyHealthBarComponent>(TEXT("HealthBarComp"));
	HealthBarComp->SetupAttachment(RootComponent);

}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("Enemy"));

	if (bTestInitialize)
	{
		LoadData();
	}
}

// =========================================================================
// IEntityStats Implementation
// =========================================================================

void ABaseEnemy::LoadData(int32 StageCount, int32 WaveCount)
{
	static const FString ContextString(TEXT("Enemy Data Context"));
	FEnemyData* EnemyData = DataTable->FindRow<FEnemyData>(*EnemyName, ContextString);

	if (EnemyData)
	{
		HealthIncStage = EnemyData->StageHealthInc;
		HealthIncWave = EnemyData->WaveHealthInc;
		DamageIncStage = EnemyData->StageDamageInc;
		DamageIncWave = EnemyData->WaveDamageInc;
		RewardIncStage = EnemyData->StageRewardInc;
		RewardIncWave = EnemyData->WaveRewardInc;

		HealthMax = EnemyData->HealthMax * (1 + (HealthIncStage * StageCount) + (HealthIncWave * WaveCount));
		Health = HealthMax;
		Defence = EnemyData->Defence;
		Damage = EnemyData->Damage * (1 + (DamageIncStage * StageCount) + (DamageIncWave * WaveCount));

		EnemyType = EnemyData->AttackType;
		AttackRange = EnemyData->AttackRange;
		AttackCooldown = EnemyData->AttackCooldown;
		ZDifferenceAllowed = EnemyData->ZDifferenceAllowed;

		MeleeAttackAngle = EnemyData->MeleeAttackAngle;
		RangeProjectileSpeed = EnemyData->RangeProjectileSpeed;
		RangeProjectileGravity = EnemyData->RangeProjectileGravity;
		RangeProjectileAOE = EnemyData->RangeProjectileAOE;

		AttackSound = EnemyData->AttackSound;
		AttackSoundMultiplier = EnemyData->AttackSoundMultiplier;
		ImpactSound = EnemyData->ImpactSound;
		ImpactSoundMultiplier = EnemyData->ImpactSoundMultiplier;
		DeadSound = EnemyData->DeadSound;
		DeadSoundMultiplier = EnemyData->DeadSoundMultiplier;

		Movespeed = EnemyData->Movespeed;
		GetCharacterMovement()->MaxWalkSpeed = Movespeed;

		GoldRaward = EnemyData->GoldRaward * (1 + (RewardIncStage * StageCount) + (RewardIncWave * WaveCount));
		ScoreRaward = EnemyData->ScoreRaward * (1 + (RewardIncStage * StageCount) + (RewardIncWave * WaveCount));

		bIsAlive = true;

		ProjectileObj = EnemyData->ProjectileData.ProjectileObj;
		MuzzleEffect = EnemyData->ProjectileData.MuzzleEffect;
		MuzzleEffectSize = EnemyData->ProjectileData.MuzzleEffectSize;
		HitGroundEffect = EnemyData->ProjectileData.HitEffectGround;
		HitGroundEffectSize = EnemyData->ProjectileData.HitEffectGroundSize;
		HitPlayerEffect = EnemyData->ProjectileData.HitEffectPlayer;
		HitPlayerEffectSize = EnemyData->ProjectileData.HitEffectPlayerSize;
	}

	if (bInitializeOnLoad)
	{
		StartAct();
	}
}

void ABaseEnemy::TakeDamage(float DamageAmount)
{
	if (!bIsAlive) return;

	float DamageCalc = DamageAmount * (100.0f / (100.0f + Defence));
	Health -= DamageCalc;

	if (HealthBarComp)
	{
		HealthBarComp->ShowAndUpdateHP(Health, HealthMax);
	}
	if (DamageTextClass)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);

		ADamageFloatingText* DamageTextActor = GetWorld()->SpawnActor<ADamageFloatingText>(
			DamageTextClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);
		if (DamageTextActor)
		{
			DamageTextActor->SetDamageValue(DamageCalc);
		}
	}

	if (Health <= 0)
	{
		Die();
	}
}

void ABaseEnemy::Die()
{
	if (!bIsAlive) return;
	bIsAlive = false;

	// 적 처치시 점수 부여 -> KirboGameMode로 전달
	if (AKirboGameMode* GM = Cast<AKirboGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnEnemyKilled(FMath::RoundToInt(ScoreRaward));
	}

	if (ADevHHUD* HUD = Cast<ADevHHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->AddGameLog(FName("Kill_Enemy"));
	}

	if (AEnemyAIControl* AICont = Cast<AEnemyAIControl>(GetController()))
	{
		AICont->StopMovement();
		AICont->UnPossess();
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);
	bIsAttackReady = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HealthBarComp)
	{
		HealthBarComp->SetVisibility(false);

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(HealthBarComp);
		}
	}

	if (DeathMontages.Num() > 0)
	{
		int32 RandomIdx = FMath::RandRange(0, DeathMontages.Num() - 1);
		PlayAnimMontage(DeathMontages[RandomIdx]);
	}
	else
	{
		Destroy();
	}

	int32 RandPlay = FMath::RandRange(0, DeadSound.Num() - 1);
	if (DeadSound[RandPlay])
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound[RandPlay], GetActorLocation(), DeadSoundMultiplier);
	}
	SetLifeSpan(10.0f);
}

// =========================================================================
// ICombatEntity Implementation
// =========================================================================

void ABaseEnemy::TryAttack()
{
	if (bIsAttackReady && bIsAlive)
	{
		bIsAttackReady = false;
		ExecuteAttackAnimation();

		FTimerHandle TimerHandle_AttackReset;
		GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, [this]() {
			bIsAttackReady = true;
			}, AttackCooldown, false);
	}
}

void ABaseEnemy::ExecuteAction(int32 ActionID)
{
	ExecuteActionAnimation();
}

// =========================================================================
// IAIBehavior Implementation
// =========================================================================

void ABaseEnemy::StartAct()
{
	bIsActive = true;
	GetCharacterMovement()->MaxWalkSpeed = Movespeed;
}

AActor* ABaseEnemy::GetTarget() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

bool ABaseEnemy::HasLineOfSight() const
{
	AActor* Target = GetTarget();
	if (!Target) return false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), AllEnemies);
	Params.AddIgnoredActors(AllEnemies);

	FVector StartLocation = GetActorLocation() + FVector(0, 0, 60.f);
	FVector EndLocation = Target->GetActorLocation();

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);

	if (!bHitSomething || (Hit.GetActor() == Target))
	{
		return true;
	}

	return false;
}

// =========================================================================
// Animation Execution (Internal)
// =========================================================================

void ABaseEnemy::ExecuteAttackAnimation()
{
	bIsAiming = true;
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void ABaseEnemy::ExecuteActionAnimation()
{
	// No action for base enemy.
}

void ABaseEnemy::ExecuteAimDone()
{
	bIsAiming = false;

	AActor* Target = GetTarget();
	if (!Target) return;
	SavedTargetLoc = Target->GetActorLocation();
}
void ABaseEnemy::ExecuteMeleeSound()
{
	if (EnemyType == EAttackType::Melee && AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation(), AttackSoundMultiplier);
	}
}

void ABaseEnemy::ExecuteAttackPoint()
{
	if (EnemyType == EAttackType::Ranged && ProjectileObj)
	{
		AActor* Target = GetTarget();
		if (!Target) return;

		FVector SpawnLocation = GetMesh()->GetSocketLocation(MuzzleName);
		FVector TargetLocation = SavedTargetLoc;

		float TargetHalfHeight = 0.0f;
		ACharacter* TargetChar = Cast<ACharacter>(Target);
		if (TargetChar && TargetChar->GetCapsuleComponent())
		{
			TargetHalfHeight = TargetChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}

		if (RangeProjectileGravity <= 0.0f)
		{
			TargetLocation.Z = SpawnLocation.Z;
		}
		else
		{
			TargetLocation.Z -= TargetHalfHeight;
			if (RangeProjectileAOE > 0.0f)
			{
				float RandomOffset = RangeProjectileAOE * 0.3f;
				TargetLocation.X += FMath::FRandRange(-RandomOffset, RandomOffset);
				TargetLocation.Y += FMath::FRandRange(-RandomOffset, RandomOffset);
			}
		}

		FRotator FireRotation;
		float FinalGravityScale = RangeProjectileGravity;

		if (RangeProjectileGravity > 0.0f)
		{
			FVector OutLaunchVelocity;
			bool bHaveSolution = false;

			for (int32 i = 0; i < 5; i++)
			{
				float CustomGravityZ = GetWorld()->GetGravityZ() * FinalGravityScale;

				bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(this, OutLaunchVelocity, SpawnLocation, TargetLocation, RangeProjectileSpeed, false, 0.0f, CustomGravityZ, ESuggestProjVelocityTraceOption::DoNotTrace);

				if (bHaveSolution) break;

				FinalGravityScale *= 0.75f;
			}

			if (bHaveSolution)
			{
				FireRotation = OutLaunchVelocity.Rotation();
			}
			else
			{
				FireRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
			}
		}
		else
		{
			FireRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
			FinalGravityScale = 0.0f;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AEnemyProjectile* Bullet = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileObj, SpawnLocation, FireRotation, SpawnParams);
		if (Bullet)
		{
			float LifeRange = (FinalGravityScale > 0.0f) ? (AttackRange * 8.0f) : (AttackRange * 1.5f);
			Bullet->InitializeProjectile(RangeProjectileSpeed, Damage, LifeRange, FinalGravityScale, RangeProjectileAOE);
			Bullet->InitializeEffects(HitGroundEffect, HitGroundEffectSize, HitPlayerEffect, HitPlayerEffectSize);
			Bullet->InitializeSounds(ImpactSound, ImpactSoundMultiplier);
		}

		FRotator MuzzleRotation = GetMesh()->GetSocketRotation(MuzzleName);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, SpawnLocation, MuzzleRotation, FVector::One() * MuzzleEffectSize, true);
		if (AttackSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation(), AttackSoundMultiplier);
		}
	}
	else if (EnemyType == EAttackType::Melee)
	{
		AActor* Target = GetTarget();
		if (Target && GetDistanceTo(Target) <= AttackRange)
		{
			FVector Forward = GetActorForwardVector();
			FVector ToTarget = (SavedTargetLoc - GetActorLocation()).GetSafeNormal();
			float DotProduct = FVector::DotProduct(Forward, ToTarget);
			float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(MeleeAttackAngle * 0.5f));

			if (DotProduct >= AngleThreshold)
			{
				UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, UDamageType::StaticClass());
				if (ImpactSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Target->GetActorLocation(), ImpactSoundMultiplier);
				}
			}
		}
	}
}
