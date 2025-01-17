// Fill out your copyright notice in the Description page of Project Settings.


#include "World/CPickUp.h"
#include "Character/CSurvivor.h"
#include "Character/CSurvivorController.h"
#include "Components/ArrowComponent.h"
#include "ActorComponents/CInventoryComponent.h"
#include "Utility/CDebug.h"
#include "Engine/DataTable.h"
#include "ActorComponents/CInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widget/Inventory/CItemBase.h"
#include "Widget/Build/CBuildWidget.h"
#include "Build/CStructure_Placeable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPickUp::ACPickUp()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	SetRootComponent(PickupMesh);
	//PickupMesh->SetSimulatePhysics(false);
	PickupMesh->SetEnableGravity(true);
	SetReplicates(true);

	ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("DataTable'/Game/PirateIsland/Include/Datas/Widget/Inventory/DT_Items.DT_Items'"));
	if (DataTableAsset.Succeeded())
	{
		ItemDataTable = DataTableAsset.Object;
	}
	bTransformTimerUse = true;
}

void ACPickUp::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UCItemBase::StaticClass(), ItemQuantity);
}

void ACPickUp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnRequestDesroyCalled.Clear(); // CPickUp 파괴되기 전에 델리게이트 바인드한 목록 삭제 
}

#if WITH_EDITOR
// 에디터 내에서 변수 값이 바뀌었을때 델리게이트로 호출됨 (언리얼 Built-in 함수)
void ACPickUp::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ACPickUp, DesiredItemID)) //바뀐 변수가 DesiredItemID면
	{
		if (ItemDataTable)
		{
			const FString ContextString = DesiredItemID.ToString();

			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString())) //해당 Row가 찾아졌을때
			{
				PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh); // 바뀐 ID에 따라 Mesh Change
			}
		}
	}
}
#endif

void ACPickUp::BeginFocus()
{
		if (PickupMesh && CustomDepthStencilValue ==0)
		{
			CustomDepthStencilValue = 252;
			PickupMesh->SetRenderCustomDepth(true); //깊이 버퍼에 메쉬를 등록 
			PickupMesh->SetCustomDepthStencilValue(252);
		}
	
	
}

void ACPickUp::EndFocus()
{
	
		if (PickupMesh&& CustomDepthStencilValue == 252)
		{
			CustomDepthStencilValue = 0;
			PickupMesh->SetRenderCustomDepth(false);
			PickupMesh->SetCustomDepthStencilValue(0);
		}

	
}

void ACPickUp::BeginInteract()
{
	
}

void ACPickUp::EndInteract()
{
}

void ACPickUp::InitializePickup(const TSubclassOf<class UCItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone()) //Empty String 인지 체크 
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());
		if (ItemData)
		{

			ItemReference = NewObject<UCItemBase>(this, BaseClass);

			ItemReference->ID = ItemData->ID;
			ItemReference->ItemType = ItemData->ItemType;
			ItemReference->NumericData = ItemData->NumericData;
			ItemReference->TextData = ItemData->TextData;
			ItemReference->AssetData = ItemData->AssetData;
			ItemReference->ItemStats = ItemData->ItemStats;
			ItemReference->ProduceData = ItemData->ProduceData;
			ItemReference->BuildData = ItemData->BuildData;
			ItemReference->HuntData = ItemData->HuntData;

			if (InQuantity <= 0) //0보다 작으면 
			{
				ItemReference->SetQuantity(1);
			}
			else
			{
				ItemReference->SetQuantity(InQuantity);
			}

			PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

			if (((ItemReference->ItemType != EItemType::Build) && (ItemReference->ItemType != EItemType::Container)))
			{
				PickupMesh->SetCollisionProfileName(FName("Item"));
				//PickupMesh->SetSimulatePhysics(false);
			}
			UpdateInteractableData();
		}
	}
}

void ACPickUp::InitializeDrop(FName ItemID, const int32 InQuantity, int32 RemainDurability)
{
	if (this->HasAuthority())
	{
		BroadCastInitializeDrop(ItemID, InQuantity, RemainDurability);
	}

}

void ACPickUp::PerformInitializeDrop(UCItemBase* ItemToDrop, const int32 InQuantity)
{
	if (ItemToDrop)
	{
		ItemReference = ItemToDrop;
		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
		ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight(); // UCItemBase에서 Item 무게 가져와 설정
		ItemReference->Inventory = nullptr;
		MeshToChange = ItemToDrop->AssetData.Mesh;
		PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);
		//DT_Items에 DropMesh가 있을 경우 DropMesh로 드롭, 없을 경우 Mesh로 드롭 (ex.) Build 아이템을 빌드 위젯으로 spawn하지 않고 메뉴에서 직접 Drop )
		if (ItemToDrop->AssetData.DropMesh)
		{
			PickupMesh->SetStaticMesh(ItemToDrop->AssetData.DropMesh);
			ItemReference->bIsDropMesh = true;

		}
		else
			PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

		if (((ItemReference->ItemType != EItemType::Build) && (ItemReference->ItemType != EItemType::Container)) || ItemReference->AssetData.DropMesh)
		{
			PickupMesh->SetCollisionProfileName(FName("Item"));
			PickupMesh->SetSimulatePhysics(true);
		}

		UpdateInteractableData();
		ApplyTransformSync();
	}
}

//void ACPickUp::RequestInitializeDrop_Implementation(FName ItemID, const int32 InQuantity, int32 RemainDurability)
//{
//	InitializeDrop(ItemID, InQuantity, RemainDurability);
//}

void ACPickUp::BroadCastInitializeDrop_Implementation(FName ItemID, const int32 InQuantity, int32 RemainDurability)
{
	const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemID, ItemID.ToString());
	if (ItemData)
	{
		UCItemBase* ItemToDrop = NewObject<UCItemBase>(StaticClass());
		ItemToDrop->CopyFromItemData(*ItemData);
		if (RemainDurability != -1)
			ItemToDrop->ItemStats.RemainDurability = RemainDurability;
		PerformInitializeDrop(ItemToDrop, InQuantity);
	}
}

void ACPickUp::BroadcastSetTransform_Implementation(FTransform InTransform)
{
	SetActorTransform(InTransform);
}

void ACPickUp::BroadcastDestroy_Implementation()
{
	Destroy();
}

void ACPickUp::BroadcastUpdatePartialAdded_Implementation(int32 InQuantity)
{
	UpdatePartialAdded(InQuantity);
}

void ACPickUp::UpdatePartialAdded(int32 InQuantity)
{
	ItemReference->SetQuantity(InQuantity);
	InteractableData.Quantity = InQuantity;
}

void ACPickUp::UpdateInteractableData()
{
	switch (ItemReference->ItemType)
	{
	case EItemType::Build:
		InstanceInteractableData.InteractableType = EInteractableType::Build;
		break;
	case EItemType::Container:
		InstanceInteractableData.InteractableType = EInteractableType::Container;
		break;
	default:
		InstanceInteractableData.InteractableType = EInteractableType::Pickup;
		break;
	}
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InstanceInteractableData.ID = ItemReference->ID;
	InstanceInteractableData.bIsDropMesh = ItemReference->bIsDropMesh; //DropMesh(주머니) 일때 참조 bool 
	InstanceInteractableData.bIsCantPickUp = ItemReference->BuildData.bIsCantPickUp;
	InstanceInteractableData.bIsDoorOpened = ItemReference->BuildData.bIsDoorOpened;
	InteractableData = InstanceInteractableData; // InteractableData 는 인터페이스에서 선언된 FInteractableData
}

void ACPickUp::Interact(ACSurvivor* PlayerCharacter, bool bIsLongPressed)
{
	if (PlayerCharacter)
	{
		if (ItemReference->ItemType == EItemType::Build)
		{
			if (bIsLongPressed)
				TakePickup(PlayerCharacter);
			else
			{
				if (InstanceInteractableData.bIsDropMesh)
				{
					TakePickup(PlayerCharacter);
				}
				else
				{
					if (ItemReference->BuildData.bIsInteractableBuildStructure)
						DoBuildTypeInteract();
				}
			}
		}
		else if(ItemReference->ItemType == EItemType::Container)
		{
			if (bIsLongPressed)
				TakePickup(PlayerCharacter);
			else
			{
				if (InstanceInteractableData.bIsCantPickUp)
				{
					OpenActorInventory(PlayerCharacter, this);
				}
				else
				{
					if (InstanceInteractableData.bIsDropMesh)
					{
						TakePickup(PlayerCharacter);
					}
					else
					{
						OpenActorInventory(PlayerCharacter, this);
					}
				}
			}
		}
		else
		{
			TakePickup(PlayerCharacter);
		}
	}
}

void ACPickUp::TakePickup(const ACSurvivor* Taker)
{

	if (!IsPendingKillPending()) //픽업 아이템이 Destroy 되고 있는지 체크 
	{
		if (UCInventoryComponent* PlayerInventory = Taker->GetInventoryComponent())
		{
			const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

			switch (AddResult.OperationResult)
			{
			case EItemAddResult::NoItemAdded:
				break;
			case EItemAddResult::PartialItemAdded:
			{
				if (Taker->HasAuthority())
				{
					BroadcastUpdatePartialAdded(ItemReference->Quantity);
				}
				else
				{
					OnUpdatePartialAdded.Broadcast(ItemReference->Quantity);
				}
				UpdateInteractableData(); //PickUp 아이템 수량 조정 
				Taker->GetInteractionComponent()->UpdateInteractionWidget(); //인터렉션 ui  업뎃
				break;
			}
			case EItemAddResult::AllItemAdded:
			{
				if (Taker->HasAuthority())
				{
					BroadcastDestroy();
				}
				else
				{
					OnRequestDesroyCalled.Broadcast();//RequestDestroy() 호출을 클라이언트에서 하도록 델리게이트로 브로드캐스트
				}
				break;
			}
			}
			
		}

		ACSurvivorController* survivorController = Cast<ACSurvivorController>(Taker->GetController());
		if (survivorController)
		{
			if (survivorController->GetBuildWidget())
			{
				survivorController->GetBuildWidget()->RefreshBuildWidgetQuantity(ItemReference->ID);
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
}

void ACPickUp::ApplyTransformSync()
{
	

	if (this->HasAuthority())
	{
		PickupMesh->SetSimulatePhysics(true);
		GetWorld()->GetTimerManager().SetTimer(TransformTimerHandle, this, &ACPickUp::SetTransform, 0.05f, true);
	}
}

void ACPickUp::OpenActorInventory(const ACSurvivor* Survivor, class AActor* Actor)
{
	// 하위 클래스 자체에서 함수내용 구현
}

void ACPickUp::DoBuildTypeInteract()
{
	// 하위 클래스 자체에서 함수내용 구현 (Door)
}



void ACPickUp::SetTransform()
{
	
	if (HasAuthority())
	{
		BroadcastSetTransform(this->GetActorTransform());
	}
}