#include "DevHWeaponSlot.h"
#include "Team7_CH3_Project/Weapon/WeaponData.h" // 무기 데이터
#include "Team7_CH3_Project/UI/DevHHUD.h"
#include "Team7_CH3_Project/Manager/KirboGameInstance.h"


void UDevHWeaponSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDevHWeaponSlot::InitSlotData()
{
    if (!WeaponHandle.IsNull())
    {
        FWeaponStat* Data = WeaponHandle.GetRow<FWeaponStat>(TEXT("Slot Init"));
        if (Data)
        {
            CachedUnlockScore = Data->UnlockScore;
            DefaultFrameColor = Data->FrameColor;

            if (Image_WeaponIcon && Data->WeaponIcon)
                Image_WeaponIcon->SetBrushFromTexture(Data->WeaponIcon);

            if (Image_Frame)
                Image_Frame->SetBrushTintColor(FSlateColor(DefaultFrameColor));

            // GameInstance에서 현재 누적 점수를 가져옴
            UKirboGameInstance* GI = Cast<UKirboGameInstance>(GetGameInstance());
            int32 PersistentScore = GI ? GI->GetTotalScore() : 0;

            // 초기화 시점에는 bSilentUpdate를 true로 설정하여 로그 출력을 방지
            UnlockStatus(PersistentScore, true);
        }
    }
}

void UDevHWeaponSlot::UnlockStatus(int32 CurrentScore, bool bSilentUpdate)
{
    if (Border_Lock)
    {
        bool bShouldBeUnlocked = CurrentScore >= CachedUnlockScore;

        if (!bSilentUpdate && !bIsUnlocked && bShouldBeUnlocked && CachedUnlockScore > 0)
        {
            if (ADevHHUD* HUD = Cast<ADevHHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
            {
                HUD->AddGameLog(FName("Weapon_Unlock"));
            }
        }

        bIsUnlocked = bShouldBeUnlocked;
        Border_Lock->SetVisibility(bIsUnlocked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
}

void UDevHWeaponSlot::SetSlotActive(bool bIsActive)
{
    // 미리 저장된 해금 상태만 확인
    if (!bIsUnlocked)
    {
        return;
    }

	if (Image_Frame)
	{
		// 선택되면 강조색, 해제되면 다시 원본 색상
		FLinearColor TargetColor = bIsActive ? SelectedFrameColor : DefaultFrameColor;
		Image_Frame->SetBrushTintColor(FSlateColor(TargetColor));
	}
}
