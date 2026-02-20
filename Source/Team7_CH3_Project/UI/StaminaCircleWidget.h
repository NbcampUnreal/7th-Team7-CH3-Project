#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaCircleWidget.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UStaminaCircleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    class UImage* StaminaBarImage;

    UPROPERTY()
    class UMaterialInstanceDynamic* StaminaMID; // 머티리얼 파라미터 조절용


protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void UpdateStamina(float CurrentStamina, float MaxStamina);
};
