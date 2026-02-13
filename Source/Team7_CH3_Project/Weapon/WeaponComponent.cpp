// Fill out your copyright notice in the Description page of Project Settings.


#include "Team7_CH3_Project/Weapon/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "WeaponData.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h" // KH 추가 : UI
#include "Team7_CH3_Project/Skill/SkillData.h" // KH 추가 : 스킬 데이터 테이블

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CurrentStat = nullptr;
	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// 쓰레기 값으로 인한 사격 차단 방지
	LastFireTime = 0.0f;
	LastWeaponSwitchTime = -WeaponSwitchDelay;
    LastSkillUsedTime = -100.0f; // KH 추가 : 시작하자마자 스킬 사용 가능하도록 초기화

	// 데이터 테이블 로드 및 초기 무기 세팅
	if (WeaponStatTable)
	{
		CurrentStat = WeaponStatTable->FindRow<FWeaponStat>(WeaponRowName, TEXT(""));
	}

	// 가져온 정보가 있다면 초기화 진행
	if (CurrentStat)
	{
		CurrentAmmo = CurrentStat->MaxAmmo;
		// 초기 무기 탄약 등록
		WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponStat NOT FOUND! Check Table or RowName."));
	}

	bIsNextShotLeft = true;

    // [UI 필수 코드! 지우지 말아주세요!] KH 추가 : 시작 시 UI에 초기 무기 정보 방송
    if (CurrentStat)
    {
        if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
        {
            UISub->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
        }
    }
	
}

void UWeaponComponent::StartFire()
{
	bHoldingFire = true; // 마우스를 눌렀음을 기록

	if (!CurrentStat || bIsReloading) return;

	// 탄약이 없으면 즉시 장전 시도
	if (CurrentAmmo <= 0)
	{
		StartReload();
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastFireTime >= CurrentStat->FireRate)
	{
		// 충분한 시간이 지났다면 즉시 발사 가능하도록 LastFireTime을 과거로 설정
		LastFireTime = CurrentTime - CurrentStat->FireRate;
	}

	// 첫 발 즉시 발사 후 FireRate 간격으로 자동 연사 타이머 작동
	Fire();

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UWeaponComponent::Fire,
		CurrentStat->FireRate,
		true
	);
}

void UWeaponComponent::StopFire()
{
	bHoldingFire = false; // 마우스를 뗐음을 기록
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UWeaponComponent::Fire()
{
    // 1. 점수 및 스탯 체크 (최우선 가드 로직)
    if (!CurrentStat || GetCurrentScore() < CurrentStat->UnlockScore)
    {
        // 점수가 부족해지면 연사 타이머를 해제하고 사격을 멈춥니다.
        GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
        return;
    }

    // 2. 상태 체크 (장전 중이거나 탄약이 없는 경우)
    if (bIsReloading || CurrentAmmo <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
        if (CurrentAmmo <= 0) StartReload();
        return;
    }

	// 마우스 연타 방지 로직 : FireRate보다 빠르게 입력이 들어오면 무시
	float CurrentTime = GetWorld()->GetTimeSeconds();

    // KH 추가: UI 서브시스템에 공격 발동 알림
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISubSystem = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 현재 무기의 연사 속도(FireRate)를 쿨타임으로 전달
            UISubSystem->BroadcastNormalAttack(CurrentStat->FireRate);

            // 기존 탄약 업데이트 방송
            UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
        }
    }

	// 무기를 바꾼 지 WeaponSwitchDelay만큼 지나지 않았다면 발사하지 않음
	if (CurrentTime - LastWeaponSwitchTime < WeaponSwitchDelay) return;
	// 타이머 오차를 고려해 FireRate의 80%만 지나도 발사를 허용 (연사 끊김 방지)
	if (CurrentTime - LastFireTime < CurrentStat->FireRate * 0.8f) return;

	AActor* OwnerActor = GetOwner();
	ACharacter* OwnerChar = Cast<ACharacter>(OwnerActor);
	if (!OwnerChar) return;

	USkeletalMeshComponent* Mesh = OwnerChar->GetMesh();
	if (!IsValid(Mesh)) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 좌우 소켓 스위칭 및 사격 시작점 설정
	FName TargetSocketName = bIsNextShotLeft ? LeftMuzzleSocketName : RightMuzzleSocketName;
	FVector Start = Mesh->GetSocketLocation(TargetSocketName);

	// 카메라가 바라보는 정면 방향을 기준으로 조준점 설정
	FVector FireDir = OwnerChar->GetActorForwardVector();

	// PelletCount 만큼 라인트레이스 반복
	int32 TotalPellets = FMath::Max(1, CurrentStat->PelletCount);
	for (int32 i = 0; i < TotalPellets; i++)
	{
		// 탄퍼짐 계산 (Cone 형태의 랜덤 방향 생성)
		float SpreadRad = FMath::DegreesToRadians(CurrentStat->BulletSpread);
		FVector SpreadDir = FMath::VRandCone(FireDir, SpreadRad);
		FVector End = Start + (SpreadDir * CurrentStat->Range);

		// 사격 판정 (라인트레이스)
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerChar);
		bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
		FVector FinalEnd = bHit ? Hit.ImpactPoint : End;

		// 맞았을 때는 빨간색, 안 맞았을 때는 초록색으로 표시
		FColor LineColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(World, Start, End, LineColor, false, 1.0f, 0, 0.5f);

		if (CurrentStat->TracerEffect)
		{
			// 나이아가라 시스템 스폰
			UNiagaraComponent* Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				CurrentStat->TracerEffect,
				Start,
				(FinalEnd - Start).Rotation()
			);

			if (Tracer)
			{
				Tracer->SetNiagaraVariablePosition(TEXT("User.Target"), FinalEnd);
			}
		}

		if (bHit)
		{
			// 맞은 지점에 작은 구체 표시
			DrawDebugSphere(World, Hit.ImpactPoint, 10.0f, 12, FColor::Red, false, 0.1f);

			AActor* HitActor = Hit.GetActor();
			if (IsValid(HitActor))
			{
				if (CurrentStat->HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->HitSound, Hit.ImpactPoint);
				}

				UGameplayStatics::ApplyDamage(HitActor, CurrentStat->Damage, OwnerChar->GetController(), OwnerChar, nullptr);
				UE_LOG(LogTemp, Log, TEXT("Hit: %s"), *HitActor->GetName());
			}

			if (CurrentStat->ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, CurrentStat->ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
	}

	// 발사 피드백 (이펙트 & 사운드)
	if (CurrentStat->MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(CurrentStat->MuzzleFlash, Mesh, TargetSocketName);
	}

	if (CurrentStat->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->FireSound, Start);
	}
	// 사격 성공 후 시간 업데이트
	LastFireTime = CurrentTime;

	// 데이터 업데이트
	CurrentAmmo--;
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);
	bIsNextShotLeft = !bIsNextShotLeft;

    // KH 추가 : UI 서브시스템
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISubSystem = GI->GetSubsystem<UDevHUISubSystem>())
        {
            // 무기 타입, Ammo 개수 방송
            UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
        }
    }
 
	if (CurrentAmmo <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		StartReload();
	}

	UE_LOG(LogTemp, Log, TEXT("Ammo: %d / %d"), CurrentAmmo, CurrentStat->MaxAmmo);
}

void UWeaponComponent::StartReload()
{
	if (bIsReloading || CurrentAmmo >= CurrentStat->MaxAmmo) return;

	bIsReloading = true;
	// 장전 사운드 재생
	if (CurrentStat->ReloadSound)
	{
		// 캐릭터의 위치에서 사운드를 재생
		UGameplayStatics::PlaySoundAtLocation(this, CurrentStat->ReloadSound, GetOwner()->GetActorLocation());
	}

	UE_LOG(LogTemp, Log, TEXT("Reloading..."), CurrentStat->ReloadTime);

	// ReloadTime 후에 CompleteReload 함수를 호출하도록 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UWeaponComponent::CompleteReload,
		CurrentStat->ReloadTime,
		false
	);
}

void UWeaponComponent::CompleteReload()
{
	CurrentAmmo = CurrentStat->MaxAmmo;
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);
	bIsReloading = false;

    // UI 서브 시스템
    if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
    }

	float CurrentTime = GetWorld()->GetTimeSeconds();
	LastFireTime = CurrentTime - CurrentStat->FireRate;

	UE_LOG(LogTemp, Log, TEXT("Reload Complete"));

	// 장전이 끝났을 때 마우스를 여전히 누르고 있다면 사격 재개
	if (bHoldingFire)
	{
		StartFire();
	}
}

void UWeaponComponent::ChangeWeapon(FName NewWeaponName)
{
	if (bIsReloading || WeaponRowName == NewWeaponName)
	{
		UE_LOG(LogTemp, Warning, TEXT("장전 중에는 무기를 바꿀 수 없거나 이미 같은 무기입니다."));
		return;
	}

	if (!WeaponStatTable) return;

	// 현재 무기의 남은 탄약을 맵에 저장
	WeaponAmmoMap.Add(WeaponRowName, CurrentAmmo);

	//  KH 추가 : 새로운 행 이름으로 데이터 찾기
	FWeaponStat* NewStat = WeaponStatTable->FindRow<FWeaponStat>(NewWeaponName, TEXT(""));

    if (NewStat)
    {
        //  KH 추가 : 헬퍼 함수를 사용하여 간결하게 체크
        if (GetCurrentScore() < NewStat->UnlockScore)
        {
            UE_LOG(LogTemp, Warning, TEXT("무기 잠김 상태: 해금 점수 부족!"));
            return;
        }

		// 현재 스탯 교체 및 변수 초기화
		WeaponRowName = NewWeaponName;
		CurrentStat = NewStat;

		//  새로운 무기의 탄약을 맵에서 불러오기
		if (WeaponAmmoMap.Contains(NewWeaponName))
		{
			CurrentAmmo = WeaponAmmoMap[NewWeaponName];
		}
		else
		{
			// 맵에 정보가 없다면(처음 드는 무기라면) 최대 탄약으로 초기화
			CurrentAmmo = CurrentStat->MaxAmmo;
			WeaponAmmoMap.Add(NewWeaponName, CurrentAmmo);
		}

		float CurrentTime = GetWorld()->GetTimeSeconds();
		LastFireTime = CurrentTime;
		LastWeaponSwitchTime = CurrentTime;

		// 사격 중이었다면 타이머 초기화 (연사 속도가 다를 수 있으므로)
		if (bHoldingFire)
		{
			StopFire();

			// WeaponSwitchDelay만큼 기다린 후 StartFire를 다시 호출
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				this,
				&UWeaponComponent::StartFire,
				WeaponSwitchDelay,
				false
			);
		}

		UE_LOG(LogTemp, Log, TEXT("Weapon Switched to: %s"), *NewWeaponName.ToString());

        // KH 추가 : 성공적으로 교체되었을 때만 UI 방송
        if (UDevHUISubSystem* UISubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
        {
            UISubSystem->TriggerWeaponSelection(NewStat->SlotIndex);

            // KH 추가 : 무기 이름과 탄약 정보 방송 & 초기 무기의 슬롯 인덱스 선택 신호 방송
            UISubSystem->BroadcastWeaponStatus(WeaponRowName.ToString(), CurrentAmmo, CurrentStat->MaxAmmo);
            UISubSystem->TriggerWeaponSelection(CurrentStat->SlotIndex);
        }
	}
}


int32 UWeaponComponent::GetCurrentScore() const // KH 추가 : 서브시스템에 저장된 점수 가져오는 보조 함수
{
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            //  KH 추가 : 서브시스템에 구현된 GetCurrentScore()를 호출
            return UISub->GetCurrentScore();
        }
    }
    return 0;
}

void UWeaponComponent::TryUseSkill() // KH 추가 : 스킬 실행 함수 (UI 쿨타임 연동)
{
    if (!SkillStatTable) return;

    // 데이터 테이블에서 스킬 정보 찾기
    FSkillData* SkillData = SkillStatTable->FindRow<FSkillData>(CurrentSkillRowName, TEXT(""));

    if (SkillData)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();

        // 쿨타임 체크 (현재 시간 - 마지막 사용 시간 >= Cooldown)
        if (CurrentTime - LastSkillUsedTime >= SkillData->Cooldown)
        {
            // --- [임시] 실제 스킬 로직을 여기에 구현 (나중에 상빈님이 채울 부분) ---
            UE_LOG(LogTemp, Warning, TEXT("스킬 사용됨: %s"), *SkillData->SkillName);
            // ----------------------------------------------------------------------

            // 마지막 사용 시점 업데이트
            LastSkillUsedTime = CurrentTime;

            // UI 서브시스템에 스킬 쿨타임 방송 (UI 게이지 작동)
            // [UI 필수 코드! 지우지 말아주세요!]
            if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
            {
                UISub->BroadcastSkillAttack(SkillData->Cooldown);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("스킬 재사용 대기 중... 남은 시간: %.1f초"), SkillData->Cooldown - (CurrentTime - LastSkillUsedTime));
        }
    }
}
