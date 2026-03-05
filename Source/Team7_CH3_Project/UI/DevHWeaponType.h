#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DevHWeaponType.generated.h"

UCLASS()
class TEAM7_CH3_PROJECT_API UDevHWeaponType : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock_WeaponType;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock_AmmoCount;

    // 원본 포맷 패턴을 저장해둘 변수
    FText WeaponNameFormat;
    FText AmmoCountFormat;

    virtual void NativeConstruct() override;


    // 서브시스템 신호 처리 함수
    UFUNCTION()
    void HandleWeaponStatusUpdate(FString WeaponName, int32 CurrentAmmo, int32 MaxAmmo);
};
