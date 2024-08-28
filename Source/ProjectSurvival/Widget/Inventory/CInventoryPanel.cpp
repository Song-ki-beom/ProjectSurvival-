// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/CInventoryPanel.h"
#include "Widget/Inventory/CInventoryItemSlot.h"
#include "Character/CSurvivor.h"
#include "Widget/Inventory/CItemBase.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "ActorComponents/CInventoryComponent.h"
#include "Widget/Inventory/CItemDragDropOperation.h"

void UCInventoryPanel::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    PlayerCharacter = Cast<ACSurvivor>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        InventoryReference = PlayerCharacter->GetInventoryComponent();

        if (InventoryReference)
        { //인벤토리의 델리게이트로부터 인벤토리 업데이트 이벤트 바인드
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UCInventoryPanel::RefreshInventory);
            SetInfoText();


        }

    }



}


//인벤토리로부터 수량과 용량 정보 업데이트 
void UCInventoryPanel::SetInfoText() 
{
    WeightInfo->SetText(FText::FromString( FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight())+ "/"+FString::SanitizeFloat(InventoryReference->GetWeightCapacity())));

    CapacityInfo->SetText(FText::Format(FText::FromString("{0}/{1}"), InventoryReference->GetInventoryContents().Num(), InventoryReference->GetSlotsCapacity()));
}




void UCInventoryPanel::RefreshInventory()
{
    if (InventoryReference && InventorySlotClass)
    {
        //WrapBox에 정보를 추가하기 전에 기존 이미지 삭제
        InventoryPanel->ClearChildren();
        
        
        //for 문 Iterate : 
        for (TWeakObjectPtr<UCItemBase> InventoryItem : InventoryReference->GetInventoryContents())
        {
            UCInventoryItemSlot* ItemSlot = CreateWidget<UCInventoryItemSlot>(this,InventorySlotClass);
            ItemSlot->SetItemReference(InventoryItem.Get());
            ItemSlot->SetHUDReference(InventoryReference->GetHUDReference());
            InventoryPanel->AddChildToWrapBox(ItemSlot);

        }
        SetInfoText();


    }
}



bool UCInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    //별도로 만든 DragDropOperation 생성 
    const UCItemDragDropOperation* ItemDragDrop = Cast<UCItemDragDropOperation>(InOperation);
    if (ItemDragDrop->SourceItem && InventoryReference) //아이템이 DragDropOperation에서 감지되면 (아이템이 Drag중이면 )
    {
        return true; // Drop 취소 
    }
    return false; 
}
