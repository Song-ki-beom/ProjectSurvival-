// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/CDestructibleActor.h"
#include "DestructibleComponent.h"
#include "Struct/CDestructibleStructures.h"

// Sets default values
ACDestructibleActor::ACDestructibleActor()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleMesh")); 
	DestructibleComponent->SetupAttachment(GetRootComponent());
}



void ACDestructibleActor::SetUp(float InMaxDamageThreshold, UDestructibleMesh* InDestructibleMesh, FTransform InstanceTransform)
{
	DestructibleComponent->SetDestructibleMesh(InDestructibleMesh);
	DestructibleComponent->SetWorldTransform(InstanceTransform);
	MaxDamageThreshold = InMaxDamageThreshold;
}

class UDestructibleComponent* ACDestructibleActor::GetDestructibleComponent()
{
	return DestructibleComponent;
}



float ACDestructibleActor::GetAccumulatedDamage()
{
	return AccumulatedDamage;
}

void ACDestructibleActor::AccumulateDamage(float DamageAmount)
{
	AccumulatedDamage += DamageAmount;
	if (AccumulatedDamage >= MaxDamageThreshold)
	{
		//
	}
}



// Called when the game starts or when spawned
void ACDestructibleActor::BeginPlay()
{
	Super::BeginPlay();
	DestructibleComponent->RegisterComponent();
	DestructibleComponent->AddToRoot(); // Root Set 등록 ->가비지 컬렉션 삭제 방지
}

void ACDestructibleActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DestructibleComponent)
	{
		DestructibleComponent->RemoveFromRoot(); // Root Set 등록 해제
		DestructibleComponent->DestroyComponent();
	}

	Super::EndPlay(EndPlayReason);
}




