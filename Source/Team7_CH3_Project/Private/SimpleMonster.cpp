#include "SimpleMonster.h"

ASimpleMonster::ASimpleMonster()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASimpleMonster::BeginPlay()
{
    Super::BeginPlay();
    CurrentHP = MaxHP;
}

void ASimpleMonster::ApplyDamage(float Damage)
{
    CurrentHP -= Damage;

    if (CurrentHP <= 0.f)
    {
        Die();
    }
}

void ASimpleMonster::Die()
{
    OnMonsterDead.Broadcast(this);
    Destroy();
}
