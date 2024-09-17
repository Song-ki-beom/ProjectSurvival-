// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSURVIVAL_API ACEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACEnemyAIController();
protected:  
	virtual void BeginPlay() override;
protected:
	void  OnPossess(APawn* InPawn)  override; 
	void  OnUnPossess()   override;
private:
	UPROPERTY(VisibleAnywhere)
		class UAIPerceptionComponent* Perception;
private:  
	UFUNCTION()
		void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private: 
	class  ACEnemy* Enemy;
	class  UAISenseConfig_Sight* Sight;
	class  UCEnemyAIComponent* AIComponent;
};
