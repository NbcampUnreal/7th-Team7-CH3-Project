#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleMonster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnMonsterDeadSignature,
    ASimpleMonster*,
    DeadMonster);

UCLASS()
class TEAM7_CH3_PROJECT_API ASimpleMonster : public ACharacter
{
    GENERATED_BODY()

public:
    ASimpleMonster();

    UPROPERTY(BlueprintAssignable)
    FOnMonsterDeadSignature OnMonsterDead;

    UFUNCTION(BlueprintCallable)
    void ApplyDamage(float Damage);

protected:
    virtual void BeginPlay() override;

private:
    float MaxHP = 100.f;
    float CurrentHP = 100.f;

    void Die();
};
