// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActorComponents/CStateComponent.h"
#include "Struct/CWeaponStructures.h"
#include "Perception/AISightTargetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "CEnemy.generated.h"



UCLASS()
class PROJECTSURVIVAL_API ACEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACEnemy();

protected:
	virtual void BeginPlay() override;


public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//DoAction
	virtual float DoAction();
	UFUNCTION(NetMulticast, Reliable)
	virtual void BroadcastDoAction(int32 InAttackIdx);
	UFUNCTION(Server, Reliable)
	virtual void RequestDoAction();
	virtual void PerformDoAction(int32 InAttackIdx);
	virtual void Begin_DoAction();
	virtual void End_DoAction();
	virtual void AttackTraceHit();
	virtual float DoEncounter();
	virtual void PerformDoSpecialAction(ESpecialState SpecialState);
	UFUNCTION(NetMulticast, Reliable)
	virtual void BroadcastDoSpecialAction(ESpecialState SpecialState);
	UFUNCTION(NetMulticast, Reliable)
	virtual void BroadcastDisableCollision();
	UFUNCTION(NetMulticast, Reliable)
	virtual void BroadcastDisableMesh();
	UFUNCTION(NetMulticast, Reliable)
	virtual void BroadcastChangeMesh();
	UFUNCTION(Server, Reliable)
	virtual void RequestMoveStop();


	UFUNCTION(NetMulticast, Reliable)
	void BroadcastUpdateHealthBar(FLinearColor InColor);
	


//Damage Interface Override
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	//Eat Food
	void EatFood(class ACPickUp* TargetPickUp);

	// 팀 ID 설정 및 반환 함수
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; };
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;


protected:
	//Montage 
	void AfterABPBindDynamic();
	virtual void ApplyHitData();
	virtual void Die();
	virtual void RemoveCharacter();
	//Material 
	virtual  void CreateDynamicMaterial();
	virtual  void ChangeMeshColor(FLinearColor InColor);
private:
	//Hit
	UFUNCTION(NetMulticast, Reliable)
		void BroadCastApplyHitData(FDamageData InDamageData);
	
	void ResetColor();
	
	//Drop Item
	void CreateDropItem();

	//Destroy
	void DestroyEnemy();

	//DelegateBindHandler
	UFUNCTION()
	void OnStateTypeChangedHandler(EStateType PrevType, EStateType NewType);
	UFUNCTION()
	void OnBecameFriendlyHandler();
	
	//Mesh Rotation
	void RotateMeshToSlope(float InDeltaTime);

	//Montage
	UFUNCTION()
	void OnMontageFinalEnded();
	UFUNCTION()
	void OnMontageInterrupted();
	UFUNCTION()
	void OnPlayMontageNotifyBegin();
	UFUNCTION()
	void OnPlayMontageNotifyEnd();


protected: // 하위 클래스에서 설정하고 동적 로딩하기 위해 Protected 설정
	//Mesh
	FString SkeletalMeshPath;
	FString AnimInstancePath; 
	FString FriendlyMeshPath;
	//Drop
	int32 DropItemNum;
	FName DropItemID;
	float DropOffsetRange;

	//DoAction
	UPROPERTY(EditAnywhere)
	TArray<FDoActionData> DoActionDatas;

	//Special Action
	UPROPERTY(EditAnywhere)
	TArray<FDoActionData> DoSpecialActionDatas;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow")
	class UArrowComponent* SlopeCheckArrow;
	// Box Collision Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxCollision;

	//Hit 
	FString HitActorName = FString("_Bear");


protected:
	UPROPERTY()
	class UCGameInstance* GameInstance;
	UPROPERTY()
	class UDataTable* ItemDataTable;
	UPROPERTY()
	class UNetDriver* NetDriver;
	UPROPERTY()
	class ACharacter* PlayerCharacter;

	//AI 
	UPROPERTY(EditAnywhere, Category = "AI")
		class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "AI")
		class ACEnemyAIController* AIController;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FGenericTeamId TeamId; // 팀 ID 저장 변수
	
	
	//Component
	UPROPERTY(VisibleAnywhere)
	class UCStatusComponent* StatusComponent;
	UPROPERTY(VisibleAnywhere)
	class UCMovingComponent* MovingComponent;
	UPROPERTY(VisibleAnywhere)
	class UCStateComponent* StateComponent;
	UPROPERTY(VisibleAnywhere)
	class UCMontageComponent* MontageComponent;
	UPROPERTY(VisibleAnywhere)
	class UCEnemyAIComponent* EnemyAIComponent;
	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource;

	//Attack
	float TraceDistance = 45.0f;
	float TraceOffset = 100.0f;
	int32 AttackIdx = 0;
	
	//Damage
	FDamageData DamageData;
	FHitData* HitData;
	FTimerHandle DieTimerHandle;
	int32 hitCnt = 0;
	int32 MaxhitCnt = 3;
	bool bHasFirstHitted = false;
	//Material
	FLinearColor OriginalMeshColor = FLinearColor::White;
	FTimerHandle ResetColorTimerHandle;

	//World HPbar
	UPROPERTY(VisibleAnywhere)
		TSubclassOf<class UUserWidget> HPBarWidgetClass;
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HPBarWidgetComponent;

	//Destroy
	FTimerHandle RemoveTimerHandle;

	bool bChangeMesh;

public: //ForceInline Getter & Settter


FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
	
};
