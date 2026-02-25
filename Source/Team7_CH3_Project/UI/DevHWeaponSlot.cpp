#include "DevHWeaponSlot.h"
#include "Team7_CH3_Project/Weapon/WeaponData.h" // 무기 데이터
#include "Team7_CH3_Project/UI/DevHHUD.h"

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
			{
				Image_WeaponIcon->SetBrushFromTexture(Data->WeaponIcon);
			}

			if (Image_Frame)
			{
				Image_Frame->SetBrushTintColor(FSlateColor(DefaultFrameColor));
			}
			UnlockStatus(0);
		}
	}
}

void UDevHWeaponSlot::UnlockStatus(int32 CurrentScore)
{
	if (Border_Lock)
	{
		// 현재 점수가 해금 점수보다 높으면 해금
		bool bIsUnLocked = CurrentScore >= CachedUnlockScore;

		if (CachedUnlockScore > 0 && bIsUnLocked && Border_Lock->GetVisibility() == ESlateVisibility::Visible)
		{
			if (ADevHHUD* HUD = Cast<ADevHHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
			{
				HUD->AddGameLog(FName("Weapon_Unlock"));
			}
		}

		Border_Lock->SetVisibility(bIsUnLocked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UDevHWeaponSlot::SetSlotActive(bool bIsActive)
{
	int32 CurrentScore = 0;
	if (CurrentScore < CachedUnlockScore)
	{
		return; // 해금X 무기
	}

	if (Image_Frame)
	{
		// 선택되면 강조색, 해제되면 다시 원본 색상
		FLinearColor TargetColor = bIsActive ? SelectedFrameColor : DefaultFrameColor;
		Image_Frame->SetBrushTintColor(FSlateColor(TargetColor));
	}
	// 선택된 슬롯 연출
	// float Scale = bIsActive ? 1.15f : 1.0f;
	// SetRenderScale(FVector2D(Scale, Scale));
}
