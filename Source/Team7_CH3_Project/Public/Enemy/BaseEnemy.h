// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IEnemy.h"
#include "Enemy/EnemyProjectile.h"
#include "Enemy/EnemyObjectData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseEnemy.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API ABaseEnemy : public ACharacter, public IEnemy
{
	GENERATED_BODY()

public:
    ABaseEnemy();

protected:
    UPROPERTY(EditAnywhere, Category = "Enemy")
    UDataTable* DataTable;
    UPROPERTY(EditAnywhere, Category = "Enemy")
    UEnemyObjectData* EnemyObjectData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Enemy")
    FString Name = "BaseEnemy";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    EAttackType EnemyType = EAttackType::Melee;

    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float Health = 100.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float HealthMax = 100.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float HealthIncStage = 0.2f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float HealthIncWave = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Attack")
    float Defence = 5.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float Movespeed = 375.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float MovespeedAct = 250.f;
    
    UPROPERTY(EditAnywhere, Category = "Enemy|Attack")
    float Damage = 10.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float DamageIncStage = 0.15f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Status")
    float DamageIncWave = 0.03f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Attack")
    float AttackRange = 100.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Attack")
    float AttackCooldown = 1.5f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Attack|Melee")
    float AttackAngle = 85.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Attack|Ranged")
    float ProjectileSpeed = 750.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Action")
    float ActionRange = 1200.f;
    UPROPERTY(EditAnywhere, Category = "Enemy|Action")
    float ActionCooldown = 5.f;
    
    UPROPERTY(EditAnywhere, Category = "Enemy|Reward")
    int GoldDrop = 20;
    UPROPERTY(EditAnywhere, Category = "Enemy|Reward")
    float itemChance = 0.2f;
    
    bool bIsAttackReady = true;
    bool bIsActionReady = true;
    FTimerHandle AttackTimers;
    FTimerHandle ActionTimers;
    
    void ResetAttackCooldown() { bIsAttackReady = true; }
    void ResetActionCooldown() { bIsActionReady = true; }
    void ResetMovespeed() { GetCharacterMovement()->MaxWalkSpeed = Movespeed; }

    void LoadData(int StageCount = 0, int WaveCount = 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animations")
    FName MuzzleName = "Muzzle_Front";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animations")
    class UAnimMontage* AttackMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animations")
    class UAnimMontage* ActionMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animations")
    TArray<UAnimMontage*> DeathMontages;

    bool bIsAlive = true;

public:
    virtual void BeginPlay() override;

    virtual FString GetName() const override { return Name; }

    virtual EAttackType GetAttackType() const override { return EnemyType; }
    virtual bool HasLineOfSight() const override;

    virtual void StartChase() override;

    virtual void TryAttack() override;
    virtual void TryAction() override;

    virtual float GetDamage() const override { return Damage; }

    virtual float GetHealth() override { return Health; }
    virtual float GetHealthMax() override { return HealthMax; }
    virtual float GetHealthPercent() override { return Health / HealthMax; }

    virtual void TakeDamage(float DamageAmount) override;

    UFUNCTION(BlueprintCallable, Category = "Enemy/AttackRelated")
    void ExecuteAttackPoint();
    void ExecuteAttackAnimation();
    void ExecuteActionAnimation();

    virtual float GetAttackRange() const override { return AttackRange; }
    virtual float GetActionRange() const override { return ActionRange; }
    virtual bool IsAttackReady() const override { return bIsAttackReady; }
    virtual bool IsActionReady() const override { return bIsActionReady; }

    virtual float GetMovespeed() const override { return Movespeed; }
    virtual AActor* GetTarget() const override;

    virtual int GetGoldDrop() const override { return GoldDrop; }
    virtual float GetItemChance() const override { return itemChance; }

    virtual void Die() override;
    virtual bool IsAlive() const override { return bIsAlive; }
};
