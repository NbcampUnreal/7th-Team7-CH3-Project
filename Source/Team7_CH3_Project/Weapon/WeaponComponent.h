// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataTable.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

// 무기의 세부 스탯을 정의하는 구조체(DataTable용)
USTRUCT(BlueprintType)
struct FWeaponStat : public FTableRowBase // 데이터 테이블 기능을 위해 상속
{
	GENERATED_BODY()

	/* --- 기본 전투 스탯 --- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Range = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float FireRate = 0.2f; // 발사 간격 (낮을수록 연사가 빠름)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 MaxAmmo = 14;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ReloadTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float BulletSpread = 1.0f; // 탄퍼짐 정도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 PelletCount = 1; // 한 번에 발사되는 탄환 수

	/* --- UI 관련 에셋 --- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* WeaponIcon; // UI에 표시될 무기 아이콘

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor FrameColor; // 등급별 프레임 색상

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 UnlockScore; // 해금에 필요한 점수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsLocked = true; // 현재 잠금 상태 (기본값 true)

	/* --- 시각/청각 피드백 에셋 --- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<UParticleSystem> MuzzleFlash; // 총구 화염

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<UParticleSystem> ImpactEffect; // 탄착점 이펙트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* TracerEffect; // 탄환 궤적 (나이아가라)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
};


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
};
