// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataTable.h"
#include "Components/ActorComponent.h"
#include "WeaponData.h"
#include "WeaponComponent.generated.h"

class UNiagaraSystem;
class UDataTable;
struct FGrenadeStat;
class AGrenadeProjectile;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM7_CH3_PROJECT_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	/** 마우스 입력 연동 함수 (입력 컴포넌트에서 호출) */

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartFire(); // 블루프린트에서 마우스 좌클릭을 눌렀을 때 호출 할 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFire(); // 블루프린트에서 마우스 좌클릭을 뗐을 때 호출 할 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartReload(); // 장전 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LaunchGrenade(); // 블루프린트에서 마우스 우클릭을 눌렀을 때 호출 할 함수

	/** 실시간 무기 교체 (데이터 테이블의 행 이름을 입력) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ChangeWeapon(FName NewWeaponName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* --- 컴포넌트 설정값 --- */

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UDataTable> WeaponStatTable; // 무기 데이터 테이블 연결을 위한 변수

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UDataTable> GrenadeTable; // 수류탄 데이터 테이블을 위한 변수

	UPROPERTY(EditAnywhere, Category = "Config")
	FName WeaponRowName = TEXT("Pistol"); // 초기 무기 설정

	UPROPERTY(EditAnywhere, Category = "Config")
	FName GrenadeRowName = TEXT("Grenade");

	UPROPERTY(EditAnywhere, Category = "Config")
	FName LeftMuzzleSocketName = TEXT("Muzzle_L");

	UPROPERTY(EditAnywhere, Category = "Config")
	FName RightMuzzleSocketName = TEXT("Muzzle_R");

	void RegenerateGrenade(); // 수류탄 충전 로직 함수

private:	

	void Fire(); // 총 발사 로직

	void CompleteReload(); // 장전 완료 시 호출

	FWeaponStat* CurrentStat; // 현재 활성화된 무기 정보

	bool bIsReloading = false; // 현재 장전 중인가?
	int32 CurrentAmmo;
	int32 CurrentGrenadeCount;
	bool bIsNextShotLeft = true; // 좌우 번갈아 쏘기 위한 플래그
	bool bHoldingFire = false; // 현재 마우스를 누르고 있는지 확인
	float LastFireTime; // 연타 방지를 위한 마지막 사격 시점 기록
	float WeaponSwitchDelay = 0.5f; // 무기 교체 후 즉시 사격을 방지하기 위한 지연 시간
	float LastWeaponSwitchTime; // 무기를 바꾼 시점을 기록
	TMap<FName, int32> WeaponAmmoMap; // 무기 이름(FName)별로 남은 탄약(int32)을 저장하는 맵
    int32 GetCurrentScore() const; // KH 추가 : 서브시스템에 저장된 점수 가져오는 보조 함수
	float LastGrenadeTime = 0.0f; // 마지막 수류탄 발사 시점 기록

	// 장전 대기 시간을 관리할 타이머 핸들
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	FTimerHandle GrenadeRegenTimerHandle; // 수류탄 충전용 타이머 핸들
};
