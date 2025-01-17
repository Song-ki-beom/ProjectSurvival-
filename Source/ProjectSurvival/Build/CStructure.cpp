#include "Build/CStructure.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utility/CDebug.h"

ACStructure::ACStructure()
{
	bReplicates = true;
	PreviewBox = CreateDefaultSubobject<UBoxComponent>("PrivewBox");
	PreviewBox->SetupAttachment(PickupMesh);
	PreviewBox->SetIsReplicated(true);
	bTransformTimerUse = false;
}

void ACStructure::BeginPlay()
{
	Super::BeginPlay();
	SaveOriginMaterial();
	PickupMesh->SetSimulatePhysics(false);
}

void ACStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void ACStructure::OpenActorInventory(const ACSurvivor* Survivor, AActor* Actor)
//{
//	Super::OpenActorInventory(Survivor, Actor);
//	CDebug::Print(TEXT("비긴인터랙트 ACStructure"), FColor::Cyan);
//}


void ACStructure::BeginFocus()
{
	Super::BeginFocus();
}

void ACStructure::EndFocus()
{
	Super::EndFocus();
}

void ACStructure::BeginInteract()
{
	Super::BeginInteract();
}

void ACStructure::EndInteract()
{
	Super::EndInteract();
}

//void ACStructure::Interact(ACSurvivor* PlayerCharacter)
//{
//	Super::Interact(PlayerCharacter);
//}

void ACStructure::BroadcastDestroyPreviewBox_Implementation()
{
	PreviewBox->DestroyComponent();
}

void ACStructure::SaveOriginMaterial()
{
	if (IsValid(PickupMesh->GetStaticMesh()))
		OriginMaterial = PickupMesh->GetStaticMesh()->GetMaterial(0);
}


