// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/EnemyData.h"
#include "Enemy/IEntityStats.h"
#include "Enemy/ICombatEntity.h"
#include "Enemy/IAIBehavior.h"
#include "BaseEnemy.generated.h"

class UEnemyHealthBarComponent;
class ADamageFloatingText;

UCLASS()
class TEAM7_CH3_PROJECT_API ABaseEnemy : public ACharacter, public IEntityStats, public ICombatEntity, public IAIBehavior
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	// =========================================================================
	// INITIALIZATION
	// =========================================================================
	UPROPERTY(EditAnywhere, Category = "Enemy")
	bool bTestInitialize = false;
	UPROPERTY(EditAnywhere, Category = "Enemy")
	bool bInitializeOnLoad = true;

	virtual void BeginPlay() override;
	// =========================================================================
	// DATA & ASSETS
	// =========================================================================

	UPROPERTY(EditAnywhere, Category = "Enemy|Data")
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, Category = "Enemy|Data")
	class UEnemyObjectData* EnemyObjectData;

	UPROPERTY(EditAnywhere, Category = "Enemy|Data")
	FString EnemyName = "BaseEnemy";

	// =========================================================================
	// CORE STATS (IEntityStats)
	// =========================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
	float HealthMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
	float Defence;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Stats")
	bool bIsAlive = true;
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Stats")
	bool bIsActive = true;
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Stats")
	bool bIsAiming = true;

	float HealthIncStage;
	float HealthIncWave;
	float DamageIncStage;
	float DamageIncWave;
	float RewardIncStage;
	float RewardIncWave;

	// =========================================================================
	// COMBAT (ICombatEntity)
	// =========================================================================
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	EAttackType EnemyType = EAttackType::Melee;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	float Damage;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	float AttackRange;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	float AttackCooldown;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	bool bIsAttackReady = true;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat")
	float ZDifferenceAllowed;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat|Melee")
	float MeleeAttackAngle;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat|Range")
	float RangeProjectileSpeed;
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat|Range")
	float RangeProjectileGravity;
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Combat|Range")
	float RangeProjectileAOE;

	FVector SavedTargetLoc;

	// =========================================================================
	// MOVEMENT & AI (IAIBehavior)
	// =========================================================================
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Movement")
	float Movespeed;

	// =========================================================================
	// ASSETS & ANIMATIONS
	// =========================================================================
	UPROPERTY(EditAnywhere, Category = "Enemy|Assets")
	class UAnimMontage* AttackMontage;

	// No action for base enemy.
	//UPROPERTY(EditAnywhere, Category = "Enemy|Assets")
	//class UAnimMontage* ActionMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy|Assets")
	TArray<class UAnimMontage*> DeathMontages;

	UPROPERTY(EditAnywhere, Category = "Enemy|Assets")
	FName MuzzleName = "Muzzle_Front";

	// =========================================================================
	// MISCS
	// =========================================================================

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Assets")
	TSubclassOf<class AEnemyProjectile> ProjectileObj;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UEnemyHealthBarComponent* HealthBarComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|UI")
	TSubclassOf<ADamageFloatingText> DamageTextClass;

	UPROPERTY(VisibleAnywhere, Category = "Misc")
	float GoldRaward;

	UPROPERTY(VisibleAnywhere, Category = "Misc")
	float ScoreRaward;

public:
	// --- IEntityStats Implementation ---
	virtual void LoadData(int32 StageCount = 0, int32 WaveCount = 0) override;
	virtual float GetHealth() const override { return Health; }
	virtual float GetHealthMax() const override { return HealthMax; }
	virtual float GetHealthPercent() const override { return (HealthMax > 0.0f) ? (Health / HealthMax) : 0.0f; }
	virtual bool IsActive() const override { return bIsActive; }
	virtual void TakeDamage(float DamageAmount) override;
	virtual bool IsAlive() const override { return bIsAlive; }
	virtual void Die() override;

	// --- ICombatEntity Implementation ---
	virtual EAttackType GetAttackType() const override { return EnemyType; }
	virtual float GetDamage() const override { return Damage; }
	virtual void TryAttack() override;
	virtual bool IsAttackReady() const override { return bIsAttackReady; }
	virtual float GetAttackRange() const override { return AttackRange; }
	virtual float GetAttackCooldown() const override { return AttackCooldown; }
	virtual float GetZDifferenceAllowed() const override { return ZDifferenceAllowed; }
	virtual bool IsAiming() const override { return bIsAiming; }
	virtual void ExecuteAction(int32 ActionID) override;

	// --- IAIBehavior Implementation ---
	virtual void StartAct() override;
	virtual AActor* GetTarget() const override;
	virtual bool HasLineOfSight() const override;
	virtual float GetMovespeed() const override { return Movespeed; }

	// --- Internal calls ---
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void ExecuteAimDone();
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void ExecuteAttackPoint();

protected:
	// --- Internal calls ---
	void ExecuteAttackAnimation();
	void ExecuteActionAnimation();
};
