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
    TArray<FGridPos> cubePositions = GatherPositionsForCubes(LocationToGridPos(loc));

    TArray<UStaticMeshComponent*> unusedCubes;
    TArray<FGridPos> unfilledPositions;
    unfilledPositions.Append(cubePositions);

    for(size_t i = 0; i < CubeCount; i++)
    {
        auto cube = CubesMeshes[i];
        FGridPos pos = LocationToGridPos(cube->GetComponentLocation());
        bool stillGoodPos = false;

        for(int j = 0; j < cubePositions.Num(); j++)
        {
            FGridPos cubePos = cubePositions[j];
            if(cubePos == pos)
            {
                stillGoodPos = true;
                unfilledPositions.Remove(cubePos);
            }
        }

        if(!stillGoodPos)
        {
            unusedCubes.Push(cube);
        }
    }

    for(int i = 0; i < unfilledPositions.Num(); i++)
    {
        auto cube = unusedCubes.Pop(false);
        //UE_LOG(LogTemp, Warning, TEXT("Number %d pos: %s"), i, *cubeLocs[i].ToString());
        cube->SetWorldLocation(GridPosToLocation(unfilledPositions[i]));
    }
}

TArray<FGridPos> ACubeGenerator::GatherPositionsForCubes(const FGridPos& centerPos)
{
    TArray<FGridPos> positions;
    positions.Empty(CubeCount);

    FGridPos startPos = centerPos - FGridPos(CubeDimCount / 2);
    for(size_t z = 0; z < CubeDimCount; z++)
    {
        for(size_t y = 0; y < CubeDimCount; y++)
        {
            for(size_t x = 0; x < CubeDimCount; x++)
            {
                FGridPos cubePos = startPos + FGridPos(x, y, z);
                positions.Emplace(cubePos);
            }
        }
    }

    return positions;
}

FGridPos ACubeGenerator::LocationToGridPos(const FVector& center)
{
    FVector gridLoc = (center / CubeSize) + (0.5f*FVector::OneVector);
    FGridPos gridPos = FGridPos(FMath::FloorToInt(gridLoc.X), FMath::FloorToInt(gridLoc.Y), FMath::FloorToInt(gridLoc.Z));
    return gridPos;
}

FVector ACubeGenerator::GridPosToLocation(const FGridPos& pos)
{
    FVector result = FVector(pos)*CubeSize;
    return result;
}
