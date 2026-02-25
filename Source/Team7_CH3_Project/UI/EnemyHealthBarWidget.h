#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "EnemyHealthBarWidget.generated.h"


UCLASS()
class TEAM7_CH3_PROJECT_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateHealthBar(float Percent);
    // 위젯의 프레임 업데이트 함수
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* EnemyHealthBar;

    float TargetPercent = 1.0f;  // 도달해야 할 목표치
    float CurrentPercent = 1.0f; // 현재 화면에 보이는 수치

    UPROPERTY(EditAnywhere, Category = "UI Settings")
    float InterpSpeed = 10.0f;
};
