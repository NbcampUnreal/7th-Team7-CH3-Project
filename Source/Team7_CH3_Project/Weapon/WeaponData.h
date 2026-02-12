#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"
#include "WeaponData.generated.h"


// 무기의 세부 스탯을 정의하는 구조체(DataTable용)
USTRUCT(BlueprintType)
struct FWeaponStat : public FTableRowBase // 데이터 테이블 기능을 위해 상속
{
    GENERATED_BODY()

public:
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
    int32 SlotIndex = 0; // 몇 번째 슬롯에 대응하는지 지정

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UTexture2D* WeaponIcon = nullptr; // UI에 표시될 무기 아이콘

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FLinearColor FrameColor = FLinearColor::White; // 등급별 프레임 색상

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int32 UnlockScore = 0; // 해금에 필요한 점수

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsLocked = true; // 현재 잠금 상태 (기본값 true)

    /* --- 시각/청각 피드백 에셋 --- */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    TObjectPtr<UParticleSystem> MuzzleFlash = nullptr; // 총구 화염

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    TObjectPtr<UParticleSystem> ImpactEffect = nullptr; // 탄착점 이펙트

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    UNiagaraSystem* TracerEffect = nullptr; // 탄환 궤적 (나이아가라)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> ReloadSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TObjectPtr<USoundBase> HitSound = nullptr;
};
