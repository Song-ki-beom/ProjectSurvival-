#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSurvivorController.generated.h"

UCLASS()
class PROJECTSURVIVAL_API ACSurvivorController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSurvivorController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	void GetSurvivor();
	void SetupBuildWidget();
	void SetupBuildComponentFunction();
	class UCBuildWidget* GetBuildWidget() { return BuildWidget; }

protected:
	void DoAction();
	void HoldAxe();

private:
	void ToggleBuildWidget();
	void SelectQ();
	void SelectW();
	void SelectE();
	void SelectA();
	void SelectS();
	void SelectD();
	void SelectZ();
	void SelectX();
	void SelectC();

	void TestP();

private:
	class ACSurvivor* CSurvivor;

	UPROPERTY()
		TSubclassOf<class UUserWidget> BuildWidgetClass;
	UPROPERTY()
		class UCBuildWidget* BuildWidget;

	class UDataTable* BuildStructureData;

	bool bIsBuildWidgetOn;
};

