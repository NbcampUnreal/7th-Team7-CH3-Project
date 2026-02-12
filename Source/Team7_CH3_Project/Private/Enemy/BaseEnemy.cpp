// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyData.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemy::ABaseEnemy()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABaseEnemy::BeginPlay()
{
    Super::BeginPlay();
    LoadData();
    StartChase();
}

void ABaseEnemy::LoadData()
{
    if (!DataTable) return;
    static const FString ContextString(TEXT("Enemy Data Context"));
    FEnemyData* EnemyData = DataTable->FindRow<FEnemyData>(*Name, ContextString);
    if (EnemyData)
    {
        EnemyType = EnemyData->AttackType;
        HealthMax = EnemyData->HealthMax;
        Health = HealthMax;
        Damage = EnemyData->Damage;
        Defence = EnemyData->Defense;
        AttackRange = EnemyData->AttackRange;
        AttackCooldown = EnemyData->AttackCooldown;
        Movespeed = EnemyData->Movespeed;
        GoldDrop = EnemyData->GoldDrop;
        itemChance = EnemyData->ItemDropChance;
        GetCharacterMovement()->MaxWalkSpeed = Movespeed;
    }
}

bool ABaseEnemy::HasLineOfSight() const
{
    AActor* Target = GetTarget();
    if (!Target) return false;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(Target);

    FVector Start = GetActorLocation() + FVector(0, 0, 60.f); 
    FVector End = Target->GetActorLocation();

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    
    return !bHit; 
}

AActor* ABaseEnemy::GetTarget() const
{
    return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABaseEnemy::StartChase()
{
    ResetMovespeed();
}

void ABaseEnemy::TryAttack()
{
    //if (!IsAttackReady() || !AttackMontage) return;
    if (!IsAttackReady()) return;

    //PlayAnimMontage(AttackMontage);
    bIsAttackReady = false;

    if (GetAttackType() == EAttackType::Melee)
    {
        // Melee attack
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("Enemy melee attack performed!"));
        }
    }
    else if (GetAttackType() == EAttackType::Ranged)
    {
        // Range attack - create enemy projectile
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("Enemy ranged attack performed!"));
        }
    }

    GetWorldTimerManager().SetTimer(AttackTimers, this, &ABaseEnemy::ResetAttackCooldown, AttackCooldown, false);
}

void ABaseEnemy::TryAction()
{
    if (!IsActionReady()) return;

    bIsActionReady = false;
    GetCharacterMovement()->MaxWalkSpeed = MovespeedAct;
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue, TEXT("Enemy Actioned!"));
    }
    // Special action performed here - empty now

    if (ActionCooldown > 0) {
        GetWorldTimerManager().SetTimer(ActionTimers, this, &ABaseEnemy::ResetActionCooldown, ActionCooldown, false);
        ResetMovespeed();
    }
}

void ABaseEnemy::TakeDamage(float DamageAmount)
{
    Health -= DamageAmount;
    if (Health <= 0.f)
    {
        Die();
    }
}

void ABaseEnemy::ExecuteAttackPoint()
{
    if (EnemyType == EAttackType::Melee)
    {
        FVector Start = GetActorLocation();
        FVector End = Start + (GetActorForwardVector() * AttackRange);
        UE_LOG(LogTemp, Warning, TEXT("Melee Damage Applied!"));
    }
    else if (EnemyType == EAttackType::Ranged && ProjectileClass)
    {
        FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_Socket"); // Place a socket on Mixamo hand
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    }
}

void ABaseEnemy::Die()
{
    // Dead animation execution
}
