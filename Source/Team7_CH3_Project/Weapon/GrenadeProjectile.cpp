// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGrenadeProjectile::AGrenadeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 충돌체 생성 및 루트 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	// 콜리전 설정 (발사체 표준)
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));

	// 외형(Mesh) 생성 및 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬는 충돌 무시 (구체만 충돌)

	// 발사체 이동 컴포넌트 생성
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// 어깨 런처용 초기 설정
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 2000.f; // 데이터 테이블에서 덮어쓰기 전 기본값
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향 조준
	ProjectileMovement->bShouldBounce = false;           // 벽에 닿으면 튕기지 않고 OnHit 발생
	ProjectileMovement->ProjectileGravityScale = 0.3f;   // 살짝만 아래로 휘게 (런처 느낌)
}

// Called when the game starts or when spawned
void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		if (SphereComp)
		{
			SphereComp->IgnoreActorWhenMoving(MyOwner, true);
		}

		if (MyOwner->GetRootComponent())
		{
			UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(MyOwner->GetRootComponent());
			if (OwnerRoot)
			{
				OwnerRoot->IgnoreActorWhenMoving(this, true);
			}
		}
	}
	// 충돌 이벤트 바인딩
	if (SphereComp)
	{
		SphereComp->OnComponentHit.AddDynamic(this, &AGrenadeProjectile::OnHit);
	}
}

void AGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 본인(캐릭터)과의 충돌은 무시 나 자신도 아닐 때만 터짐
	if (OtherActor && OtherActor != GetOwner() && OtherActor != this)
	{
		Explode();
	}
}

void AGrenadeProjectile::Explode()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionEffect, GetActorLocation(), GetActorRotation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	// 물리적인 밀쳐내기 효과 추가
	TArray<FHitResult> HitResults;
	FVector Start = GetActorLocation();
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

	// 주변의 물리 컴포넌트들을 찾기
	bool bHasOverlap = World->SweepMultiByChannel(
		HitResults, Start, Start, FQuat::Identity,
		ECC_WorldDynamic, SphereShape);

	if (bHasOverlap)
	{
		for (const FHitResult& Hit : HitResults)
		{
			UPrimitiveComponent* HitComp = Hit.GetComponent();
			// 물리 시뮬레이션 중인 컴포넌트에만 힘을 가함
			if (HitComp && HitComp->IsSimulatingPhysics())
			{
				// AddRadialImpulse: 중심점에서 바깥쪽으로 힘을 전달
				HitComp->AddRadialImpulse(
					Start,
					ExplosionRadius,
					ImpulseStrength, // 힘의 세기 
					ERadialImpulseFalloff::RIF_Linear,
					true // 벨로시티 변화량으로 적용 (질량 무시)
				);
			}
		}
	}

	// 카메라 쉐이크 실행
	if (ExplosionCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(
			World,
			ExplosionCameraShake,
			GetActorLocation(),
			0.f,               // Inner Radius (최대 흔들림 범위)
			ExplosionRadius * 2.f, // Outer Radius (흔들림이 멈추는 범위)
			1.f                // Falloff (거리에 따른 감쇄)
		);
	}

	// 거리별 차등 데미지 적용
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,
		MaxDamage,             // Base Damage (중심부)
		MinDamage,      // Minimum Damage (가장자리 최소 데미지)
		GetActorLocation(), // 폭발 중심
		InnerRadius, // Inner Radius (100% 데미지 구간)
		ExplosionRadius,    // Outer Radius (데미지 종료 구간)
		1.0f,               // Damage Falloff (거리에 따른 감소 지수)
		nullptr,
		TArray<AActor*>(),
		this,
		GetInstigatorController(),
		ECC_Visibility
	);

	// 시각적 디버깅 (범위 확인용)
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f);

	Destroy(); // 폭발 후 제거
}
