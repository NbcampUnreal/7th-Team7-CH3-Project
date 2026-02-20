#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleMonster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ASimpleMonster*, Monster);

UCLASS()
class TEAM7_CH3_PROJECT_API ASimpleMonster : public ACharacter
{
    GENERATED_BODY()

public:
    ASimpleMonster();

    UPROPERTY(BlueprintAssignable)
    FOnMonsterDeath OnMonsterDeath;

    void SetMeshByName(const FString& MeshName);

    UFUNCTION()
    void TakeDamageSimple(float DamageAmount);

private:
    float MaxHP = 100.f;
    float CurrentHP = 100.f;

    void Die();
};
