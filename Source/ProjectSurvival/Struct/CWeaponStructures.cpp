// Fill out your copyright notice in the Description page of Project Settings.


#include "Struct/CWeaponStructures.h"
#include "GameFramework/Character.h"
#include "ActorComponents/CStateComponent.h"
#include "ActorComponents/CMovingComponent.h"
#include "ActorComponents/CMontageComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/CapsuleComponent.h"
#include "Utility/CDebug.h"
#include "Engine/NetworkObjectList.h"
#include "Engine/PackageMapClient.h"
#include "ActorComponents/CMontageComponent.h"

void FDoActionData::DoAction(ACharacter* InOwner)
{
	UCMovingComponent* movingComponent = Cast<UCMovingComponent>(InOwner->GetComponentByClass(UCMovingComponent::StaticClass()));
	if (!!movingComponent)
	{
		if (this->bFixedCamera)
			movingComponent->EnableFixedCamera();
		if (!this->bCanMove)
			movingComponent->Stop();
	}

	if (this->Montage)
	{
		UCMontageComponent* montageComponent;
		montageComponent = Cast<UCMontageComponent>(InOwner->GetComponentByClass(UCMontageComponent::StaticClass()));
		if (!!montageComponent)
			montageComponent->Montage_Play(this->Montage, this->PlayRate);

			//몽타주 컴포넌트가 없을 시 플레이 
			//InOwner->PlayAnimMontage(this->Montage, this->PlayRate);
	}
}

void FHitData::SendDamage(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOther)
{

	FActionDamageEvent e;
	e.HitID = ID;
	InOther->TakeDamage(this->DamageAmount, e, InAttacker->GetController(), InAttackCauser);
}

void FHitData::PlayMontage(ACharacter* InOwner)
{
	if(Montage== nullptr ) return;
	if(InOwner == nullptr) return;

	UCMontageComponent* montagesComponent;
	montagesComponent = Cast<UCMontageComponent>(InOwner->GetComponentByClass(UCMontageComponent::StaticClass()));

	if (!!montagesComponent)
		montagesComponent->Montage_Play(this->Montage, this->PlayRate);

}



void FHitData::PlaySoundWave(ACharacter* InOwner)
{
	if(Sound==nullptr) return;
	if(InOwner== nullptr) return;

	UWorld* world = InOwner->GetWorld();
	FVector location = InOwner->GetActorLocation();
	if(HitSoundAttenuation)
		UGameplayStatics::SpawnSoundAtLocation(world, Sound, location, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, HitSoundAttenuation);
	else
		UGameplayStatics::SpawnSoundAtLocation(world, Sound, location);

}


void FHitData::PlayEffect(UWorld* InWorld, const FVector& InLocation, const FRotator& InRotation, USkeletalMeshComponent* InMesh, FName InSocketName)
{
	if (InMesh == nullptr || Effect == nullptr) return;
	FTransform  transform;
	transform.SetLocation(InLocation + InRotation.RotateVector(EffectLocation));

	//파티클 스폰 
	UParticleSystem* particle = Cast<UParticleSystem>(Effect);
	FVector  location = transform.GetLocation();
	FRotator rotation = FRotator(transform.GetRotation());
	FVector scale = transform.GetScale3D();
	if (!!particle)   
		UGameplayStatics::SpawnEmitterAttached(particle, InMesh, InSocketName, location, rotation, scale);

}

void FHitData::PlayCameraShake(APlayerController* PlayerController, float InScale)
{
	
	if (PlayerController && PlayerController->IsLocalController())
	{
		PlayerController->ClientStartCameraShake(CameraShakeClass, InScale);
	}
}

AActor* FHitData::FindActorByNetGUID(FNetworkGUID NetGUID , UWorld* World)
{
	UNetDriver* NetDriver = World->GetNetDriver();

	
	if (NetDriver && NetDriver->GuidCache)
	{
		UObject* FoundObject = NetDriver->GuidCache->GetObjectFromNetGUID(NetGUID, true);  // bIgnoreFailures = true
		AActor* FoundActor = Cast<AActor>(FoundObject);
		if (FoundActor)
			return FoundActor;
		else 
			return nullptr;
	}
	return nullptr;
}
