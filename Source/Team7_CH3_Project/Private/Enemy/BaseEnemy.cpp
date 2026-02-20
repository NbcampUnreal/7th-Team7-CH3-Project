// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyData.h"
#include "Enemy/EnemyAIControl.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

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

void ABaseEnemy::LoadData(int StageCount, int WaveCount)
{
    if (!DataTable) return;
    static const FString ContextString(TEXT("Enemy Data Context"));
    FEnemyData* EnemyData = DataTable->FindRow<FEnemyData>(*Name, ContextString);
    if (EnemyData)
    {
        EnemyType = EnemyData->AttackType;
        Defence = EnemyData->Defence;
        AttackRange = EnemyData->AttackRange;
        ActionRange = EnemyData->ActionRange;
        AttackCooldown = EnemyData->AttackCooldown;
        Movespeed = EnemyData->Movespeed;
        MovespeedAct = EnemyData->MovespeedAct;
        AttackAngle = EnemyData->MeleeAttackAngle;
        ProjectileSpeed = EnemyData->RangeProjectileSpeed;
        GoldDrop = EnemyData->GoldDrop;
        itemChance = EnemyData->ItemDropChance;
        GetCharacterMovement()->MaxWalkSpeed = Movespeed;

        HealthIncStage = EnemyData->StageHealthInc;
        HealthIncWave = EnemyData->WaveHealthInc;
        DamageIncStage = EnemyData->StageDamageInc;
        DamageIncWave = EnemyData->WaveDamageInc;

        float HealthCount = EnemyData->HealthMax + (EnemyData->HealthMax * ((StageCount * HealthIncStage) + (WaveCount * HealthIncWave)));
        float DamageCount = EnemyData->Damage + (EnemyData->Damage * ((StageCount * DamageIncStage) + (WaveCount * DamageIncWave)));

        HealthMax = HealthCount;
        Health = HealthCount;
        Damage = DamageCount;
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
    if (!IsAttackReady() || !AttackMontage) return;

    bIsAttackReady = false;

    GetWorldTimerManager().SetTimer(AttackTimers, this, &ABaseEnemy::ResetAttackCooldown, AttackCooldown, false);

    ExecuteAttackAnimation();
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
    Health -= DamageAmount * (100 / (100 + Defence));
    if (Health <= 0.f)
    {
        Die();
    }
}

void ABaseEnemy::ExecuteAttackPoint()
{
    if (!EnemyObjectData) return;

    if (EnemyType == EAttackType::Melee)
    {
        FVector Center = GetActorLocation();
        float Radius = AttackRange;
        float HalfAngleDegree = AttackAngle;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

        TArray<AActor*> IgnoreActors;
        IgnoreActors.Add(this);

        TArray<AActor*> OutActors;
        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            Center,
            Radius,
            ObjectTypes,
            nullptr,
            IgnoreActors,
            OutActors
        );

        FVector Forward = GetActorForwardVector();

        for (AActor* HitActor : OutActors)
        {
            FVector DirToTarget = (HitActor->GetActorLocation() - Center).GetSafeNormal();
            float DotProduct = FVector::DotProduct(Forward, DirToTarget);
            float CosAngle = FMath::Cos(FMath::DegreesToRadians(HalfAngleDegree));

            if (DotProduct >= CosAngle)
            {
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    Damage,
                    GetController(),
                    this,
                    UDamageType::StaticClass()
                );
            }
        }
    }
    else if (EnemyType == EAttackType::Ranged && EnemyObjectData->BulletObj)
    {
        FVector SpawnLocation = GetMesh()->GetSocketLocation(MuzzleName);

        FVector TargetLocation = GetTarget()->GetActorLocation();
        TargetLocation.Z = SpawnLocation.Z;
        FRotator FireRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        AEnemyProjectile* Bullet = GetWorld()->SpawnActorDeferred<AEnemyProjectile>(
            EnemyObjectData->BulletObj, FTransform(FireRotation, SpawnLocation));

        if (Bullet)
        {
            Bullet->InitializeProjectile(ProjectileSpeed, Damage, AttackRange * 1.5f);
            Bullet->FinishSpawning(FTransform(FireRotation, SpawnLocation));
        }
    }
}

void ABaseEnemy::ExecuteAttackAnimation()
{
    PlayAnimMontage(AttackMontage);
}
void ABaseEnemy::ExecuteActionAnimation()
{
    PlayAnimMontage(ActionMontage);
}

void ABaseEnemy::Die()
{
    if (DeathMontages.Num() == 0)
    {
        Destroy();
        return;
    }

    if (AEnemyAIControl* AICont = Cast<AEnemyAIControl>(GetController()))
    {
        AICont->StopMovement();
        AICont->UnPossess();
        AICont->Destroy();
    }

    GetWorldTimerManager().ClearAllTimersForObject(this);

    bIsAttackReady = false;
    bIsActionReady = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

    if (DeathMontages.Num() > 0)
    {
        int32 RandomIdx = FMath::RandRange(0, DeathMontages.Num() - 1);
        PlayAnimMontage(DeathMontages[RandomIdx]);
    }

    bIsAlive = false;
    SetLifeSpan(10.0f);
}
