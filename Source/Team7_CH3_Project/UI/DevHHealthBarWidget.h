#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"  // HP
#include "Character/KirboStatRow.h" // 캐릭터 데이터 테이블 
#include "Components/TextBlock.h"
#include "DevHHealthBarWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar; // HP
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextBlock_Health; // HP 텍스트

    FText HealthFormat; // "{CurrentHP} / {MaxHP}" 형식 저장

    float TargetPercent = 1.0f; // 최종적으로 도달해야 할 목표 퍼센트
    float CurrentPercent = 1.0f; // 현재 화면에 보여지고 있는 퍼센트
    UPROPERTY(EditAnywhere, Category = "UI Settings")
    float InterpSpeed = 5.0f; // 보간 속도

public:
    UFUNCTION()
    void UpdateHealthBar(float CurrentHP, float MaxHP); // HP 업데이트
};
