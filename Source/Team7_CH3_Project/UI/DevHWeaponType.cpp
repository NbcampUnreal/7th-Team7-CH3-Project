#include "DevHWeaponType.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"


void UDevHWeaponType::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Error, TEXT("WeaponType UI: I am Alive!")); // 이 로그가 뜨는지 확인!
    // 초기 실행 시 WBP에 적힌 원본 패턴 저장
    if (TextBlock_WeaponType) WeaponNameFormat = TextBlock_WeaponType->GetText();
    if (TextBlock_AmmoCount) AmmoCountFormat = TextBlock_AmmoCount->GetText();

    if (UGameInstance* GI = GetGameInstance()) // 서브시스템 신호 받아오기
    {
        if (UDevHUISubSystem* UISubSystem = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 델리게이트 연결
            UISubSystem->OnWeaponStatusChanged.AddDynamic(this, &UDevHWeaponType::HandleWeaponStatusUpdate);
        }
    }
}

void UDevHWeaponType::HandleWeaponStatusUpdate(FString WeaponName, int32 CurrentAmmo, int32 MaxAmmo)
{
    UE_LOG(LogTemp, Warning, TEXT("UI: Signal Received! Name: %s, Ammo: %d"), *WeaponName, CurrentAmmo);

    // 무기 이름 업데이트
    if (TextBlock_WeaponType)
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("WeaponName"), FText::FromString(WeaponName)); // {WeaponName} 자리에 실제 데이터 매칭

        TextBlock_WeaponType->SetText(FText::Format(WeaponNameFormat, Args)); // 패턴에 맞춰 변환된 텍스트 적용
    }

    // 탄약 수 업데이트
    if (TextBlock_AmmoCount)
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("CurrentAmmo"), FText::AsNumber(CurrentAmmo));
        Args.Add(TEXT("TotalAmmo"), FText::AsNumber(MaxAmmo));

        TextBlock_AmmoCount->SetText(FText::Format(AmmoCountFormat, Args));
    }
}
