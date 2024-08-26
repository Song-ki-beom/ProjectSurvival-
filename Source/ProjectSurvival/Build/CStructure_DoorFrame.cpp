#include "Build/CStructure_DoorFrame.h"
#include "Kismet/KismetSystemLibrary.h"

ACStructure_DoorFrame::ACStructure_DoorFrame()
{
	PreviewBox = CreateDefaultSubobject<UBoxComponent>("PreviewBox");
	UpBox = CreateDefaultSubobject<UBoxComponent>("UpBox");

	PreviewBox->SetupAttachment(StaticMesh);
	UpBox->SetupAttachment(StaticMesh);
}

void ACStructure_DoorFrame::CheckCenter()
{
	FHitResult centerBoxHitResult;
	FVector centerBoxLocation = this->GetActorLocation();
	FVector centerBoxSize = FVector(135, 10, 135);
	FRotator centerBoxOrientation;
	if (!bDown_FoundationHit)
		centerBoxOrientation = GetOwner()->GetActorRotation();
	else
		centerBoxOrientation = CenterRotation;
	ETraceTypeQuery centerBoxTraceTypeQuery = ETraceTypeQuery::TraceTypeQuery2;
	bool bCenterBoxTraceComplex = false;
	TArray<AActor*> centerBoxActorsToIgnore;
	TArray<FHitResult> centerBoxHitResults;
	bCenterHit = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		centerBoxLocation,
		centerBoxLocation,
		centerBoxSize,
		centerBoxOrientation,
		centerBoxTraceTypeQuery,
		bCenterBoxTraceComplex,
		centerBoxActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		centerBoxHitResult,
		true,
		FLinearColor::Green,
		FLinearColor::Red
	);
}

void ACStructure_DoorFrame::CheckDown_Foundation()
{
	FHitResult downHitResult;
	FVector downStartLocation = this->GetActorLocation();
	FVector downEndLocation = this->GetActorLocation() + this->GetActorUpVector() * -250.0f;
	TArray<TEnumAsByte<EObjectTypeQuery>> downObjectTypeQuery;
	downObjectTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));

	TArray<AActor*> downBoxActorsToIgnore;
	downBoxActorsToIgnore.Add(this);

	bDown_FoundationHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		downStartLocation,
		downEndLocation,
		downObjectTypeQuery,
		false,
		downBoxActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		downHitResult,
		true,
		FLinearColor::Green,
		FLinearColor::Red
	);

	if (bDown_FoundationHit)
	{
		//DrawDebugLine(GetWorld(), downHitResult.GetComponent()->GetComponentLocation(), downHitResult.GetComponent()->GetComponentLocation() + downHitResult.ImpactNormal * 300.0f, FColor::Blue);
		this->SetActorLocation(downHitResult.GetComponent()->GetComponentLocation() + downHitResult.GetComponent()->GetForwardVector() * 15.0f + downHitResult.ImpactNormal * 225.0f);
		CenterRotation = downHitResult.GetComponent()->GetComponentRotation() + FRotator(0, -90, 0);
		this->SetActorRotation(CenterRotation);
	}
}

void ACStructure_DoorFrame::DestroyPreviewBox()
{
	PreviewBox->DestroyComponent();
}
