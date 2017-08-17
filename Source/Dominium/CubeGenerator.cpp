// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeGenerator.h"

#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"


// Sets default values
ACubeGenerator::ACubeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    GridOrigo = FVector(0.0, 0.0, 0.0);
    SceneComponent->SetWorldLocation(GridOrigo);
    RootComponent = SceneComponent;
    
    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Dominium/DebugCubeMaterial.DebugCubeMaterial'"));
    UMaterial* cubeMaterial;

    if(Material.Object != NULL)
    {
        cubeMaterial = (UMaterial*)Material.Object;

        for(int i = 0; i < CubeCount; i++)
        {
            FString compName = FString("CubeMesh") + FString::FromInt(i);
            UStaticMeshComponent* CubeMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(*compName));
            CubeMeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            CubeMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
            CubeMeshComp->SetStaticMesh(CubeMesh);

            CubeMeshComp->SetRelativeScale3D(4.0f*FVector::OneVector);

            //UMaterialInstanceDynamic* cubeMaterialDyn = UMaterialInstanceDynamic::Create(cubeMaterial, this);
            CubeMeshComp->SetMaterial(0, cubeMaterial);

            CubesMeshes.Add(CubeMeshComp);

            FVector firstCubePos = (0 - 1.0f*CubeSize)*FVector::OneVector;
            float x = (float)(i % CubeDimCount);
            float y = (float)((i / CubeDimCount) % CubeDimCount);
            float z = (float)(i / (CubeDimCount*CubeDimCount));
            FVector newloc(firstCubePos.X+x*CubeSize, firstCubePos.Y + y*CubeSize, firstCubePos.Z + z*CubeSize);
            //CubeMeshComp->SetWorldLocation(newloc);
            CubeMeshComp->SetRelativeLocation(newloc);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Couldn't load in debug cube material"));
    }

}

// Called when the game starts or when spawned
void ACubeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Get the GenerationCenterPoint
    auto playerController = GetWorld()->GetFirstPlayerController();
    if(playerController == nullptr)
        return;

    auto p = playerController->GetPawn();
    if(p == nullptr)
        return;

    FVector loc = p->GetActorLocation();
    TArray<FVector> cubeLocs = GatherLocationsForCubes(LocationToGridPos(loc));

    for(int i = 0; i < CubeCount; i++)
    {
        auto cube = CubesMeshes[i];
        auto mesh = cube->GetStaticMesh();

        FVector min, max;
        cube->GetLocalBounds(min, max);

        FVector scale = cube->GetComponentScale();
        FVector pos = cube->GetComponentLocation();
        
        FVector worldMin, worldMax;
        worldMin = pos + min*scale;
        worldMax = pos + max*scale;

        //UE_LOG(LogTemp, Warning, TEXT("Number %d bounds min: %s, max: %s, pos: %s"), i, *worldMin.ToString(), *worldMax.ToString(), *pos.ToString());

        FVector firstCubePos = loc - ((1.0f*CubeSize)*FVector::OneVector);
        float x = (float)(i % CubeDimCount);
        float y = (float)((i / CubeDimCount) % CubeDimCount);
        float z = (float)(i / (CubeDimCount*CubeDimCount));
        FVector newloc(firstCubePos.X + x*CubeSize, firstCubePos.Y + y*CubeSize, firstCubePos.Z + z*CubeSize);
        cube->SetWorldLocation(newloc);
    }
}

FGridPos ACubeGenerator::LocationToGridPos(const FVector& center)
{
    FVector gridLoc = (center / CubeSize) + (0.5f*FVector::OneVector);
    FGridPos gridPos = FGridPos(FMath::FloorToInt(gridLoc.X), FMath::FloorToInt(gridLoc.Y), FMath::FloorToInt(gridLoc.Z));
    return gridPos;
}

TArray<FVector> ACubeGenerator::GatherLocationsForCubes(const FGridPos& centerPos)
{
    TArray<FVector> positions;
    return positions;
}
