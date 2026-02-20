#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DevHUISubSystem.generated.h"


// 델리게이트 선언: 게임 결과를 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameResultChanged,
    bool, bIsClear
);
// 델리게이트 선언: 4개의 인자(Stage, Wave, Kills, Score)를 전달하는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHUDDataChanged,
    int32, Stage,
    int32, Wave,
    int32, Kills,
    int32, Score
);
// 델리게이트 선언: 무기 선택 바뀌었음을 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSelectionChanged,
    int32, SelectedIndex
);
// 델리게이트 선언: 무기 종류별 탄약 변화를 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponStatusChanged,
    FString, WeaponName,
    int32, CurrentAmmo,
    int32, MaxAmmo
);
// 델리게이트 선언: 체력 변화를 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged,
    float, CurrentHP,
    float, MaxHP
);
// 델리게이트 선언: 공격 쿨타임 시간을 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackCooldownTriggered,
    float, CooldownTime
);
// 델리게이트 선언: 스킬 쿨타임 시간을 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillCooldownTriggered,
    float, CooldownTime
);
// 델리게이트 선언: 스테미나 변화를 알리는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged,
    float, CurrentStamina,
    float, MaxStamina
);


UCLASS()
class TEAM7_CH3_PROJECT_API UDevHUISubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    // HUD가 해당 이벤트 주목하고 있음 (HUD가 구독할 방송 채널)
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnGameResultChanged OnGameResultChanged;
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnHUDDataChanged OnHUDDataChanged;
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnWeaponSelectionChanged OnWeaponSelectionChanged;
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnHPChanged OnHPChanged;
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnWeaponStatusChanged OnWeaponStatusChanged;
    UPROPERTY(BlueprintAssignable, Category = "UI|Events")
    FOnAttackCooldownTriggered OnAttackCooldownTriggered;
    UPROPERTY(BlueprintAssignable, Category = "UI|Events")
    FOnSkillCooldownTriggered OnSkillCooldownTriggered;
    UPROPERTY(BlueprintAssignable, Category = "UI | Events")
    FOnStaminaChanged OnStaminaChanged;

    // 신호 발신 함수 (로직에서 호출)
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void TriggerGameResult(bool bisClear); // 어디서든 함수 호출 시 UI 작동
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastHUDUpdate(int32 Stage, int32 Wave, int32 Kills, int32 Score); // 데이터 변했을 때 호출
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void TriggerWeaponSelection(int32 SelectedIndex); // 무기 바뀌었을 때
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastHPUpdate(float CurrentHP, float MaxHP); // 체력 변했을 때 호출
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastWeaponStatus(FString WeaponName, int32 CurrentAmmo, int32 MaxAmmo); // 총알 수 변했을 때 호출
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastNormalAttack(float Cooldown); // 일반 공격 작동하면 호출
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastSkillAttack(float Cooldown); // 스킬 작동하면 호출
    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void BroadcastStaminaUpdate(float CurrentStamina, float MaxStamina); // 스테미나 변하면 호출

public:
    UPROPERTY(BlueprintReadOnly, Category = "UI | Data")
    int32 CurrentScore = 0; // 점수 저장할 변수

    UFUNCTION(BlueprintCallable, Category = "UI | Function")
    void AddScore(int32 Amount); // 점수 변했을 때 호출
    UFUNCTION(BlueprintPure, Category = "UI | Function")
    int32 GetCurrentScore() const { return CurrentScore; } // 현재 점수 반환 함수
};
