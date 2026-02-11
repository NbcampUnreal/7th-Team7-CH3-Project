// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DevHWeaponSlot.h"
#include "DevHWeaponInventory.generated.h"


UCLASS()
class TEAMPROJECTSAMPLE_API UDevHWeaponInventory : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    UDevHWeaponSlot* WBP_WeaponSlot; // 1번 슬롯
    UPROPERTY(meta = (BindWidget))
    UDevHWeaponSlot* WBP_WeaponSlot_1; // 2번 슬롯
    UPROPERTY(meta = (BindWidget))
    UDevHWeaponSlot* WBP_WeaponSlot_2; // 3번 슬롯

private:
    UPROPERTY();
    TArray<UDevHWeaponSlot*> WeaponSlotArray; // 위젯 순회, 관리하는 배열

protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION()
    void SelectWeaponSlot(int32 SelectedIndex); // 무기가 선택되었을 때 UI 강조
    UFUNCTION()
    void HandleScoreUpdate(int32 Stage, int32 Wave, int32 Kills, int32 Score);
    void UpdateWeaponSlot(int32 Index, int32 CurrentScore);
    void InitializeInventory(); // 배열을 순회하며 각 슬롯의 데이터를 초기화
};
