#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Engine/DataTable.h"
#include "DevHWeaponSlot.generated.h"


UCLASS()
class TEAM7_CH3_PROJECT_API UDevHWeaponSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditAnywhere, Category = "UI | Weapon")
    FDataTableRowHandle WeaponHandle; // 슬롯에 넣을 무기 데이터 선택
    UPROPERTY(EditAnywhere, Category = "UI | Weapon")
    FLinearColor SelectedFrameColor = FLinearColor::White; // 선택 시 시각 효과

    UPROPERTY(meta = (BindWidget))
    UImage* Image_WeaponIcon; // 무기 아이콘 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Frame; // 칸 프레임 이미지
    UPROPERTY(meta = (BindWidget))
    UBorder* Border_Lock; // 자물쇠 아이콘 보더

private:
    FLinearColor DefaultFrameColor; // 원본 프레임 색상
    int32 CachedUnlockScore = 0; // 해금 점수를 저장해둘 변수

protected:
    virtual void NativeConstruct() override;


public:
    void InitSlotData(); // 게임 시작 시 딱 한 번만 호출될 초기화 함수
    void UnlockStatus(int32 CurrentScore); // 점수에 따른 해금 상태 갱신
    void SetSlotActive(bool bIsActive); // 현재 슬롯 선택 시 시각 효과
};
