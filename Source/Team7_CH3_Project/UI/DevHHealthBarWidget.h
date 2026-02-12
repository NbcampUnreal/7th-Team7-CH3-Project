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

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar; // HP
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextBlock_Health; // HP 텍스트

    FText HealthFormat; // "{CurrentHP} / {MaxHP}" 형식 저장

public:
    UFUNCTION()
    void UpdateHealthBar(float CurrentHP, float MaxHP); // HP 업데이트
};
