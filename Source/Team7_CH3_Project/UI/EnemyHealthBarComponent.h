#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBarComponent.generated.h"


UCLASS()
class TEAM7_CH3_PROJECT_API UEnemyHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
    UEnemyHealthBarComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // 몬스터의 현재 체력 비율을 받아 UI를 갱신하고 보여주는 함수
    void ShowAndUpdateHP(float CurrentHP, float MaxHP);

private:
    void HideHealthBar();

    FTimerHandle HideTimerHandle;

    UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
    float DisplayDuration = 1.0f;
};
