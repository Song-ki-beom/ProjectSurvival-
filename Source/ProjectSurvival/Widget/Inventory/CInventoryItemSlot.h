// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CInventoryItemSlot.generated.h"

UENUM()
enum class ERightClickStartWidget : uint8
{
	SurvivorInventory,
	WorkingBenchInventory,
	HideActionButtonWidget,
	None
};

UENUM()
enum class EDragDropResult : uint8
{
	InventoryToInventory,
	InventoryToPlaceable,
	InventoryToQuickSlot,
	PlaceableToPlaceable,
	PlaceableToInventory,
	PlaceableToQuickSlot,
	QuickSlotToQuickSlot,
	QuickSlotToInventory,
	QuickSlotToPlaceable
};

UCLASS()
class PROJECTSURVIVAL_API UCInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()
	



protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override; //Drag&Drop 시, Drop 하려는 Slot 의 Type 이 Drag 했을 때의 Type과 같다면, 해당 Item 을 Drop함 



public:
	void ToggleTooltip();
	bool Split(int32 InputNum);



protected:

	UPROPERTY(VisibleAnywhere,Category = "Inventory Slot")
	class UCItemBase* ItemReference;
//FORCEINLINE Getter & Setter for ItemReference 
public:
	void SetItemReference(class UCItemBase* ItemIn);
	

	
	FORCEINLINE class UCItemBase* GetItemReference()
	{ return ItemReference; }

	FORCEINLINE void SetHUDReference(class ACMainHUD* InHUDReference)
	{
		HUDReference = InHUDReference;
	}

	int32 GetUniqueItemIndexInWrapBox() { return UniqueItemIndexInWrapBox; }
	void SetUniqueItemIndexInWrapBox(int32 InIndex) { UniqueItemIndexInWrapBox = InIndex; }
	void SetItemQuantityText(int32 InQuantity);
	void SetRemainDurability(int32 InDurability);
	void SetRedXVisibility(ESlateVisibility InVisibility);

protected:


	//Edit In Instance 
	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	TSubclassOf<class UCDragItemVisual> DragItemVisualClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<class UCInventoryTooltip> ToolTipClass;



	//VisibleAnywhere
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	class UBorder* ItemBorder; //ItemSlot 의 외부 프레임 

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	class UImage* ItemIcon; //아이템을 나타내는 Icon

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	class UTextBlock* ItemQuantity; // 수량 

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	class UProgressBar* DurabilityProgressBar; // 내구도 

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	class UImage* RedX; //내구도 0인 장비 표시하는 텍스쳐 

	UPROPERTY(VisibleAnywhere, Category = "HUD")
	class ACMainHUD* HUDReference; //SubMenu를 표시할 HUD 

	UPROPERTY()
	class UCInventoryTooltip* ToolTip;


private:
	bool bIsTooltipToggled =false;
	int32 UniqueItemIndexInWrapBox;

	UPROPERTY()
		UUserWidget* OwnerWidget;
	EDragDropResult DragDropResult;
	ERightClickStartWidget RightClickStartWidget = ERightClickStartWidget::None;
};
