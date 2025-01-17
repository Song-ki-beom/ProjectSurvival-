// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Menu/CInventoryMenu.h"
#include "Widget/CMainHUD.h"
#include "Widget/Inventory/CItemDragDropOperation.h"
#include "Widget/Inventory/CItemBase.h"
#include "Widget/Inventory/CQuickSlot.h"
#include "Widget/Build/CBuildWidget.h"
#include "Widget/Produce/CProduceWidget.h"
#include "Widget/Chatting/CChattingBox.h"
#include "CGameInstance.h"
#include "Character/CSurvivor.h"
#include "Character/CSurvivorController.h"
#include "ActorComponents/CInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Input/Reply.h"
#include "Utility/CDebug.h"

void UCInventoryMenu::NativeOnInitialized() 
{

	Super::NativeOnInitialized();
}

void UCInventoryMenu::NativeConstruct()
{

	Super::NativeConstruct();

	PlayerCharacter = Cast<ACSurvivor>(GetOwningPlayerPawn());

}

FReply UCInventoryMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	if (InKeyEvent.GetKey() == EKeys::I)
	{
		//OnMainMenuToggled.Broadcast();  // 메뉴를 켜고 끄는 함수 호출
		ACMainHUD* mainHUD = Cast<ACMainHUD>(GetOwningPlayer()->GetHUD());
		if (mainHUD)
			mainHUD->SetWidgetVisibility(EWidgetCall::CloseWidget);
		return FReply::Handled();  // 입력을 처리했다고 반환
	}

	if (InKeyEvent.GetKey() == EKeys::E)
	{
		ACMainHUD* mainHUD = Cast<ACMainHUD>(GetOwningPlayer()->GetHUD());
		if (!mainHUD)
			return FReply::Unhandled();

		if (mainHUD->GetSurvivorProduceWidget() && mainHUD->GetSurvivorProduceWidget()->GetVisibility() == ESlateVisibility::Visible)
		{
			mainHUD->GetSurvivorProduceWidget()->StartProduce();
			return FReply::Handled();
		}

		if (mainHUD->GetPlaceableProduceWidget() && mainHUD->GetPlaceableProduceWidget()->GetVisibility() == ESlateVisibility::Visible)
		{
			mainHUD->GetPlaceableProduceWidget()->StartProduce();
			return FReply::Handled();
		}

		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		UCGameInstance* gameInstance = Cast<UCGameInstance>(GetGameInstance());
		if (gameInstance)
		{
			if (gameInstance->ChattingBox)
			{
				gameInstance->ChattingBox->SetInputMode();
			}
			else
			{
				CDebug::Print("gameInstance->ChattingBox is not Valid");
			}
		}
		else
		{
			CDebug::Print("gameInstance is not Valid");
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

bool UCInventoryMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//별도로 만든 DragDropOperation 생성 
	const UCItemDragDropOperation* ItemDragDrop = Cast<UCItemDragDropOperation>(InOperation);
	if (PlayerCharacter && ItemDragDrop->SourceItem) // 해당 UI 내에서 떨어뜨릴 아이템이 감지되면 
	{
		if (ItemDragDrop->DragStartWidget == WBP_InventoryPanel)
		{
			int32 remainDurability = ItemDragDrop->SourceItem->ItemStats.RemainDurability;
			PlayerCharacter->GetInventoryComponent()->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity, remainDurability);
			ACMainHUD* mainHUD = Cast<ACMainHUD>(GetOwningPlayer()->GetHUD());
			if (mainHUD)
				mainHUD->GetSurvivorProduceWidget()->RefreshProduceDetail();

			ACSurvivorController* survivorController = Cast<ACSurvivorController>(this->GetOwningPlayer());
			if (survivorController)
			{
				if (survivorController->GetBuildWidget())
				{
					survivorController->GetBuildWidget()->RefreshBuildWidgetQuantity(ItemDragDrop->SourceItem->ID);
				}
				else
				{
					CDebug::Print("survivorController->GetBuildWidget() is not Valid", FColor::Red);
				}
			}
			else
			{
				CDebug::Print("survivorController is not Valid", FColor::Red);
			}
		}
		else
		{
			ACMainHUD* mainHUD = Cast<ACMainHUD>(GetOwningPlayer()->GetHUD());
			if (mainHUD)
			{
				UUserWidget* quickSlot = Cast<UUserWidget>(mainHUD->GetQuickSlotWidget());
				if (quickSlot)
				{
					if (ItemDragDrop->DragStartWidget == quickSlot)
					{
						mainHUD->GetQuickSlotWidget()->ProcessDragToInventoryMenu(ItemDragDrop->SourceItem);
						ACSurvivor* survivor = Cast<ACSurvivor>(this->GetOwningPlayerPawn());
						if (survivor)
						{
							if (survivor->GetWeaponComponent()->GetUsingWeapon() == ItemDragDrop->SourceItem)
							{
								if (survivor->GetWeaponComponent()->GetWeaponType() != EWeaponType::Max)
									survivor->GetWeaponComponent()->SetMode(ItemDragDrop->SourceItem->HuntData.WeaponType);
							}
						}
					}
				}
			}
		}

		return true; 
	}
	return false;
}