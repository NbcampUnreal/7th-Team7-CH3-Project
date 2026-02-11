#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"  // HP
// #include "" // 캐릭터 데이터 테이블 
#include "Components/TextBlock.h"
#include "DevHHealthBarWidget.generated.h"

UCLASS()
class TEAMPROJECTSAMPLE_API UDevHHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar; // HP
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextBlock_Health; // HP 텍스트


    UPROPERTY(EditAnywhere, Category = "UI | Data")
    class UDataTable* StatTable; // UI에서 참조할 데이터 테이블

    float CachedMaxHP = 100.0f; // 현재 UI가 들고 있는 최대 체력 정보

    FText HealthFormat; // "{CurrentHP} / {MaxHP}" 형식 저장

public:
    UFUNCTION(BlueprintCallable, Category = "UI | Health UI")
    void InitializeHealth(FName CharacterRowName);
    UFUNCTION()
    void UpdateHealthBar(float CurrentHP, float MaxHP); // HP 업데이트
};
