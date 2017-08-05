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
}

void AFloorGenerator::OnConstruction(const FTransform& Transform)
{
    if(FloorMesh != nullptr && FloorMeshAsset != nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("attaching floormeshasset"));
        FloorMesh->SetStaticMesh(FloorMeshAsset);
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

    auto playerController = GetWorld()->GetFirstPlayerController();
    auto p = playerController->GetPawn();
    if(p != nullptr)
    {
        auto loc = p->GetActorLocation();

        // UE_LOG(LogTemp, Log, TEXT("location of player: %f, %f, %f"), loc.X, loc.Y, loc.Z);

        if(FloorMesh != nullptr)
        {
            FVector min, max;
            FloorMesh->GetLocalBounds(min, max);
            FVector currentloc = FloorMesh->GetComponentLocation();
            FVector newloc = currentloc;
            newloc.Z = 0.0f;

            //Do they overlap?
            int maxIterCount = 20;
            int iterCount = 0;
            bool xOverlap = newloc.X < loc.X && newloc.X + max.X > loc.X;
            bool yOverlap = newloc.Y < loc.Y && newloc.Y + max.Y > loc.Y;
            while((!xOverlap || !yOverlap) && (iterCount < maxIterCount))
            {
                iterCount++;

                if(loc.X < newloc.X)
                    newloc.X -= max.X;
                else if(loc.X > newloc.X+max.X)
                    newloc.X += max.X;
                if(loc.Y < newloc.Y)
                    newloc.Y -= max.Y;
                else if(loc.Y > newloc.Y+max.Y)
                    newloc.Y += max.Y;             

                xOverlap = newloc.X < loc.X && newloc.X + max.X > loc.X;
                yOverlap = newloc.Y < loc.Y && newloc.Y + max.Y > loc.Y;
            }

            UE_LOG(LogTemp, Log, TEXT("bounds of FloorMesh: min: %f, %f, %f, max: %f, %f, %f"), min.X, min.Y, min.Z, max.X, max.Y, max.Z);
            UE_LOG(LogTemp, Log, TEXT("location of player: %f, %f, %f"), loc.X, loc.Y, loc.Z);
            UE_LOG(LogTemp, Log, TEXT("location of FloorMesh: %f, %f, %f"), newloc.X, newloc.Y, newloc.Z);
            //UE_LOG(LogTemp, Log, TEXT("location of relloc: %f, %f, %f"), relloc.X, relloc.Y, relloc.Z);
            FloorMesh->SetWorldLocation(newloc);
        }
    }
}

