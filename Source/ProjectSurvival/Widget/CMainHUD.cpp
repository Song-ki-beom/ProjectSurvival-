#include "Widget/CMainHUD.h"
#include "Widget/Menu/CInventoryMenu.h"
#include "Widget/Inventory/CInteractionWidget.h"
#include "Widget/Inventory/CInventorySubMenu.h"
#include "Widget/Inventory/CInventoryItemSlot.h"
#include "Widget/Inventory/CInventoryPanel_WorkingBench.h"
#include "Widget/Produce/CProduceWidget.h"
#include "Widget/Produce/CProduceWidget.h"
#include "Widget/Inventory/CEarnInfoPanel.h"
#include "Widget/Inventory/CItemBase.h"
#include "Widget/Chatting/CChattingBox.h"
#include "Character/CSurvivorController.h"
#include "Utility/CDebug.h"

ACMainHUD::ACMainHUD()
{
}

void ACMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UCInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1); //그보다 아래
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
		InteractionWidget->bIsFocusable = true;
	}

	//위젯들 등록 
	if (InventoryMenuClass)
	{
		//Widget 은 그래픽 요소를 지니고 있기 때문에 StaticClass() 가 아니라 에디터에 존재하는 요소를 참조로 가져와야 한다..InventoryMenuClass 가 그 예시
		SurvivorInventoryWidget = CreateWidget<UCInventoryMenu>(GetWorld(), InventoryMenuClass);
		SurvivorInventoryWidget->AddToViewport(5); //그려지는 zOrder 최상위 , 최우선으로 Interact 하기 위해 
		SurvivorInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		SurvivorInventoryWidget->bIsFocusable = true;
	}
	
	//위젯들 등록 
	if (InventorySubMenuClass)
	{
		InventorySubMenuWidget = CreateWidget<UCInventorySubMenu>(GetWorld(), InventorySubMenuClass);
		InventorySubMenuWidget->OnFocusOnSubMenuEnded.AddUObject(this, &ACMainHUD::HideSubMenu);
	}

	if (ProduceWidgetClass)
	{
		ProduceWidget = CreateWidget<UCProduceWidget>(GetWorld(), ProduceWidgetClass);
		ProduceWidget->AddToViewport(5);
		ProduceWidget->SetVisibility(ESlateVisibility::Collapsed);
		ProduceWidget->bIsFocusable = true;
	}

	if (EarnInfoPanelClass)
	{
		EarnInfoPanel = CreateWidget<UCEarnInfoPanel>(GetWorld(), EarnInfoPanelClass);
		EarnInfoPanel->AddToViewport(4);
		EarnInfoPanel->SetVisibility(ESlateVisibility::Visible);
		EarnInfoPanel->bIsFocusable = true;
	}
}

void ACMainHUD::SetWidgetVisibility(EWidgetCall InWidgetCall, class UUserWidget* InWidget, class AActor* InActor)
{
	CDebug::Print("SetWidgetVisibility Called");

	switch (InWidgetCall)
	{
	case EWidgetCall::Survivor:
	{
		CDebug::Print("EWidgetCall::Survivor");
		DisplaySurvivorInventoryWidget();
		DisplayProduceWidget(InWidgetCall);
		const FInputModeUIOnly InputMode;// 마우스와 키보드 입력이 UI에만 영향 
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
		break;
	}
	case EWidgetCall::WorkBench:
	{
		DisplaySurvivorInventoryWidget();
		DisplayProduceWidget(InWidgetCall);
		DisplayActorInventory(InWidgetCall, InWidget, InActor);
		const FInputModeUIOnly InputMode;// 마우스와 키보드 입력이 UI에만 영향 
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
		break;
	}
	case EWidgetCall::CloseWidget:
	{
		CDebug::Print("CloseWidget Called");
		if (SurvivorInventoryWidget)
		{
			SurvivorInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (ProduceWidget)
		{
			ProduceWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (ActorInventoryWidget)
		{
			ActorInventoryWidget->RemoveFromViewport();
			ActorInventoryWidget = nullptr;
		}
		const FInputModeGameOnly InputMode;// 마우스와 키보드 입력이 InGame Action에만 반영
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
		break;
	}
	}
}

void ACMainHUD::DisplaySurvivorInventoryWidget()
{
	if (SurvivorInventoryWidget)
	{
		CDebug::Print("SurvivorInventoryWidget is valid");
		SurvivorInventoryWidget->SetVisibility(ESlateVisibility::Visible);
		SurvivorInventoryWidget->SetKeyboardFocus();
	}
	else
		CDebug::Print("SurvivorInventoryWidget is not valid");
}

void ACMainHUD::DisplayProduceWidget(EWidgetCall InWidgetCall)
{
	if (ProduceWidget)
	{
		FVector2D widgetSize = FVector2D(580, 850);
		ProduceWidget->SetDesiredSizeInViewport(widgetSize);
		ProduceWidget->SetVisibility(ESlateVisibility::Visible);
		ProduceWidget->SetKeyboardFocus();
		ProduceWidget->RefreshProduceDetail();
		ProduceWidget->SetWidgetSwitcherIndex(static_cast<int32>(InWidgetCall)); // 0번 : 생존자, 1번: 작업대
	}
}

void ACMainHUD::DisplayActorInventory(EWidgetCall InWidgetCall, class UUserWidget* InWidget, class AActor* InActor)
{
	if (InWidget)
	{
		ActorInventoryWidget = InWidget;

		switch (InWidgetCall)
		{
		case EWidgetCall::WorkBench:
		{
			FVector2D widgetSize = FVector2D(590, 440);
			FVector2D viewportSize;
			GEngine->GameViewport->GetViewportSize(viewportSize);
			ActorInventoryWidget->SetAlignmentInViewport(FVector2D(0.5f, 1.0f));
			ActorInventoryWidget->SetPositionInViewport(FVector2D(viewportSize.X / 2, viewportSize.Y / 2 - 15.0f));
			ActorInventoryWidget->SetDesiredSizeInViewport(widgetSize);
			ActorInventoryWidget->bIsFocusable = true;
			ActorInventoryWidget->SetVisibility(ESlateVisibility::Visible);
			ActorInventoryWidget->AddToViewport(5);

			UCInventoryPanel_WorkingBench* workingBenchInventory = Cast<UCInventoryPanel_WorkingBench>(ActorInventoryWidget);
			if (workingBenchInventory)
			{
				CDebug::Print("workingBenchInventory is Valid", FColor::Green);

				if (InActor)
				{
					//workingBenchInventory->SetOwnerActor(InActor);
				}
				else
					CDebug::Print("InActor is not Valid");
			}
			else
				CDebug::Print("workingBenchInventory is not Valid", FColor::Red);
		}
		}
	}
}

void ACMainHUD::ShowInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACMainHUD::HideInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

//InteractionComponent에서 트레이스 후 새로운 상호작용 가능 액터를 검출할때마다 UI 갱신
void ACMainHUD::UpdateInteractionWidget(const FInteractableData* InteractableData)
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);

		}
		InteractionWidget->UpdateWidget(InteractableData);
	}
}

void ACMainHUD::ShowSubMenu(FVector2D Position , UCInventoryItemSlot* InSlotReference)
{
	if (InventorySubMenuWidget && !InventorySubMenuWidget->IsInViewport())
	{
		
		//CDebug::Print(FText::Format(FText::FromString("{0} , {1}"), Position.X, Position.Y).ToString());
		// 서브메뉴의 위치를 설정

		InventorySubMenuWidget->SetPositionInViewport(Position, true);
		// 서브메뉴 표시
		InventorySubMenuWidget->AddToViewport(6);
		InventorySubMenuWidget->bIsFocusable = true;
		InventorySubMenuWidget->SetKeyboardFocus();
		InventorySubMenuWidget->SetSlotReference(InSlotReference);
		InventorySubMenuWidget->UpdateSubMenu();
	}

}

void ACMainHUD::HideSubMenu()
{
	if (InventorySubMenuWidget &&InventorySubMenuWidget->IsInViewport())
	{
		// 서브메뉴를 뷰포트에서 제거
		InventorySubMenuWidget->RemoveFromViewport();
	}

}

void ACMainHUD::ToggleHiddenMenu()
{
	InteractionWidget->ToggleHiddenMenu();
}

void ACMainHUD::ShowHiddenMenu()
{
	InteractionWidget->ShowHiddenMenu();
}

void ACMainHUD::HideHiddenMenu()
{
	InteractionWidget->HideHiddenMenu();
}

void ACMainHUD::ExtraOptionButtonUp()
{
	InteractionWidget->MoveFocusToPrevButton();
}

void ACMainHUD::ExtraOptionButtonDown()
{
	InteractionWidget->MoveFocusToNextButton();
}

void ACMainHUD::AddEarnedInfo(UObject* EarnedItem)
{
	EarnInfoPanel->AddEarnedItemSlot(EarnedItem);
}