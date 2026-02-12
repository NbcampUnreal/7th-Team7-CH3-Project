#include "DevHWeaponSlot.h"
// #include "FWeaponState.h" // 무기 데이터 테이블 구조체 헤더

void UDevHWeaponSlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDevHWeaponSlot::InitSlotData()
{
    if (!WeaponHandle.IsNull())
    {
        // DT 행 데이터 가져오기
        FWeaponState* Data = WeaponHandle.GetRow< FWeaponState>(TEXT("Slot Init"));
        if (Data)
        {
            // 변수에 미리 캐싱(저장)
            CachedUnlockScore = Data->UnlockScore;
            DefaultFrameColor = Data->FrameColor;

            // 아이콘 설정
            if (Image_WeaponIcon) Image_WeaponIcon->SetBrushFromTexture(Data->WeaponIcon);
            // 기본 프레임 색상 저장, 설정
            if (Image_Frame) Image_Frame->SetColorAndOpacity(DefaultFrameColor);

            UnlockStatus(0); // 초기 잠금 상태 시각화
        } 
    }
}

void UDevHWeaponSlot::UnlockStatus(int32 CurrentScore)
{
    if (Border_Lock)
    {
        // 현재 점수가 해금 점수보다 높으면 해금
        bool bIsUnLocked = CurrentScore >= CachedUnlockScore;
        Border_Lock->SetVisibility(bIsUnLocked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
    
}

void UDevHWeaponSlot::SetSlotActive(bool bIsActive)
{
    if (Image_Frame)
    {
        // 선택되면 강조색, 해제되면 다시 원본 색상
        FLinearColor TargetColor = bIsActive ? SelectedFrameColor : DefaultFrameColor;
        Image_Frame->SetColorAndOpacity(TargetColor);
    }
}