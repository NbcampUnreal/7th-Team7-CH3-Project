#include "SimpleMonster.h"

ASimpleMonster::ASimpleMonster()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASimpleMonster::SetMeshByName(const FString& MeshName)
{
    if (MeshName.IsEmpty()) return;

    FString Path = FString::Printf(
        TEXT("/Game/ParagonMinions/Characters/Minions/%s.%s"),
        *MeshName,
        *MeshName
    );

    USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, *Path);

    if (LoadedMesh)
    {
        GetMesh()->SetSkeletalMesh(LoadedMesh);
    }
}

void ASimpleMonster::TakeDamageSimple(float DamageAmount)
{
    CurrentHP -= DamageAmount;

    if (CurrentHP <= 0.f)
    {
        Die();
    }
}

void ASimpleMonster::Die()
{
    OnMonsterDeath.Broadcast(this);
    Destroy();
}
