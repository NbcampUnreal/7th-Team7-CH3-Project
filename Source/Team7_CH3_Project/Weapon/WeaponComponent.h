// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataTable.h"
#include "Components/ActorComponent.h"
#include "WeaponData.h"
#include "Team7_CH3_Project/Skill/SkillData.h" // KH 추가
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM7_CH3_PROJECT_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	/** 마우스 입력 연동 함수 (입력 컴포넌트에서 호출) */

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartFire(); // 블루프린트에서 마우스를 눌렀을 때 호출 할 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFire(); // 블루프린트에서 마우스를 뗐을 때 호출 할 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartReload(); // 장전 함수

    /** [UI 필수 코드! 코드 수정해도 이름 유지해주세요!] KH 추가 : 스킬 실행 함수 (UI 쿨타임 연동) */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryUseSkill();

	/** 실시간 무기 교체 (데이터 테이블의 행 이름을 입력) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ChangeWeapon(FName NewWeaponName);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* --- 컴포넌트 설정값 --- */

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UDataTable> WeaponStatTable; // 데이터 테이블 연결을 위한 변수

	UPROPERTY(EditAnywhere, Category = "Config")
	FName WeaponRowName = TEXT("Pistol"); // 초기 무기 설정

	UPROPERTY(EditAnywhere, Category = "Config")
	FName LeftMuzzleSocketName = TEXT("Muzzle_L");

	UPROPERTY(EditAnywhere, Category = "Config")
	FName RightMuzzleSocketName = TEXT("Muzzle_R");

    // --- [UI 필수 코드! 지우지 말아주세요!] KH : 스킬 설정 추가 ---
    UPROPERTY(EditAnywhere, Category = "Config | Skill")
    TObjectPtr<UDataTable> SkillStatTable; // 스킬 데이터 테이블
    UPROPERTY(EditAnywhere, Category = "Config | Skill")
    FName CurrentSkillRowName = TEXT("DefaultSkill"); // 사용할 스킬 행 이름


private:	

	void Fire(); // 총 발사 로직

	void CompleteReload(); // 장전 완료 시 호출

	FWeaponStat* CurrentStat; // 현재 활성화된 무기 정보

	bool bIsReloading = false; // 현재 장전 중인가?
	int32 CurrentAmmo;
	bool bIsNextShotLeft = true; // 좌우 번갈아 쏘기 위한 플래그
	bool bHoldingFire = false; // 현재 마우스를 누르고 있는지 확인
	float LastFireTime; // 연타 방지를 위한 마지막 사격 시점 기록
	float WeaponSwitchDelay = 0.5f; // 무기 교체 후 즉시 사격을 방지하기 위한 지연 시간
	float LastWeaponSwitchTime; // 무기를 바꾼 시점을 기록
	TMap<FName, int32> WeaponAmmoMap; // 무기 이름(FName)별로 남은 탄약(int32)을 저장하는 맵

	// 장전 대기 시간을 관리할 타이머 핸들
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;


private :
    int32 GetCurrentScore() const; // KH 추가 : 서브시스템에 저장된 점수 가져오는 보조 함수
    float LastSkillUsedTime = 0.0f; // [UI 필수 코드! 지우지 말아주세요!] KH 추가 : 스킬 쿨타임 관리를 위한 마지막 사용 시간 기록 
};
