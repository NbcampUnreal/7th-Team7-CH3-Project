#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackStatus.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UAttackStatus : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    class UAttackSlotWidget* WBP_AttackSlot_Normal;
    UPROPERTY(meta = (BindWidget))
    class UAttackSlotWidget* WBP_AttackSlot_Skill;

    // 에디터에서 딱 2개만 지정할 이미지
    UPROPERTY(EditAnywhere, Category = "Settings")
    UTexture2D* NormalAttackTexture;

    UPROPERTY(EditAnywhere, Category = "Settings")
    UTexture2D* SkillTexture;

    UPROPERTY(EditAnywhere, Category = "UI Settings | Color")
    FLinearColor NormalHighlightColor = FLinearColor::Blue;

    UPROPERTY(EditAnywhere, Category = "UI Settings | Color")
    FLinearColor SkillHighlightColor = FLinearColor::Blue;

protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION()
    void TriggerNormalAttackUI(float Delay);
    UFUNCTION()
    void TriggerSkillUI(float Delay);
    void ResetFeedbackColor();
};
