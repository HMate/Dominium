// Fill out your copyright notice in the Description page of Project Settings.

#include "FloorGenerator.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

#include "Engine/StaticMesh.h"

// Sets default values
AFloorGenerator::AFloorGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Attach a scene componet to root
    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    SceneComponent->SetWorldLocation(FVector(0.0, 0.0, 0.0));
    RootComponent = SceneComponent;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    FloorMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    FloorMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh2"));
    FloorMesh2->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

    ActiveFloorMesh = FloorMesh;
    PassiveFloorMesh = FloorMesh2;
    PassiveFloorMesh->SetVisibility(false);
}

void AFloorGenerator::OnConstruction(const FTransform& Transform)
{
    if(FloorMesh != nullptr && FloorMesh2 != nullptr && FloorMeshAsset != nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("attaching floormeshasset"));
        FloorMesh->SetStaticMesh(FloorMeshAsset);
        FloorMesh2->SetStaticMesh(FloorMeshAsset);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("NOT attaching floormeshasset"));
    }
}

// Called when the game starts or when spawned
void AFloorGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloorGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(FloorMesh == nullptr || FloorMesh2 == nullptr)
        return;

    auto playerController = GetWorld()->GetFirstPlayerController();
    auto p = playerController->GetPawn();
    if(p == nullptr)
        return;
    
    auto loc = p->GetActorLocation();

    FVector min, max;
    FloorMesh->GetLocalBounds(min, max);
    FVector activeFloorLoc = ActiveFloorMesh->GetComponentLocation();

    if(!IsOverlapping(activeFloorLoc, max, loc))
    {
        FVector newloc = activeFloorLoc;
        newloc.Z = 0.0f;

        //Do they overlap?
        int maxIterCount = 20;
        int iterCount = 0;
        while(!IsOverlapping(newloc, max, loc) && (iterCount < maxIterCount))
        {
            iterCount++;

            if(loc.X < newloc.X)
                newloc.X -= max.X;
            else if(loc.X > newloc.X + max.X)
                newloc.X += max.X;
            if(loc.Y < newloc.Y)
                newloc.Y -= max.Y;
            else if(loc.Y > newloc.Y + max.Y)
                newloc.Y += max.Y;
        }

        bool needsNewPlace = (iterCount > 0);
        if(needsNewPlace)
        {
            //UE_LOG(LogTemp, Log, TEXT("location of player: %f, %f, %f"), loc.X, loc.Y, loc.Z);
            //UE_LOG(LogTemp, Log, TEXT("location of FloorMesh2: %f, %f, %f"), newloc.X, newloc.Y, newloc.Z);
            //UE_LOG(LogTemp, Log, TEXT("location of relloc: %f, %f, %f"), relloc.X, relloc.Y, relloc.Z);
            PassiveFloorMesh->SetVisibility(true);
            PassiveFloorMesh->SetWorldLocation(newloc);

            auto temp = PassiveFloorMesh;
            PassiveFloorMesh = ActiveFloorMesh;
            ActiveFloorMesh = temp;

            PassiveFloorMesh->SetVisibility(false);
        }
    }
     
}

bool AFloorGenerator::IsOverlapping(const FVector& iPos, const FVector& iBounds, const FVector& iTargetPoint)
{
    bool xOverlap = iPos.X < iTargetPoint.X && (iPos.X + iBounds.X) > iTargetPoint.X;
    bool yOverlap = iPos.Y < iTargetPoint.Y && (iPos.Y + iBounds.Y) > iTargetPoint.Y;
    return xOverlap && yOverlap;
}
