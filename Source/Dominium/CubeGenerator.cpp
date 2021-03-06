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
    
    CubesMeshes.Empty(CubeCount);

    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Dominium/DebugCubeMaterial.DebugCubeMaterial'"));
    UMaterial* cubeMaterial;

    if(Material.Object != NULL)
    {
        cubeMaterial = (UMaterial*)Material.Object;

        for(int i = 0; i < CubeCount; i++)
        {
            FString compName = FString("CubeMesh") + FString::FromInt(i);

            UTerrainVolumetricChunkComponent* volumeComp = CreateDefaultSubobject<UTerrainVolumetricChunkComponent>(FName(*compName));
            volumeComp->mFrequency = mNoiseFrequency;
            volumeComp->mZBias = mZBias;

            CubesMeshes.Add(volumeComp);

            CubeSize = volumeComp->GetSize().X;

            FVector firstCubePos = GridOrigo - ((CubeDimCount / 2)*FVector(CubeSize));
            float x = (float)(i % CubeDimCount);
            float y = (float)((i / CubeDimCount) % CubeDimCount);
            float z = (float)(i / (CubeDimCount*CubeDimCount));
            FVector newloc(firstCubePos.X + x*CubeSize, firstCubePos.Y + y*CubeSize, firstCubePos.Z + z*CubeSize);
            volumeComp->SetWorldLocation(newloc);
            volumeComp->GenerateBlock();

            /*
            UStaticMeshComponent* CubeMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(*compName));
            CubeMeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            CubeMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
            CubeMeshComp->SetStaticMesh(CubeMesh);

            //UMaterialInstanceDynamic* cubeMaterialDyn = UMaterialInstanceDynamic::Create(cubeMaterial, this);
            CubeMeshComp->SetMaterial(0, cubeMaterial);
            CubesMeshes.Add(CubeMeshComp);


            FVector firstCubePos = GridOrigo - FVector(CubeSize);
            float x = (float)(i % CubeDimCount);
            float y = (float)((i / CubeDimCount) % CubeDimCount);
            float z = (float)(i / (CubeDimCount*CubeDimCount));
            FVector newloc(firstCubePos.X+x*CubeSize, firstCubePos.Y + y*CubeSize, firstCubePos.Z + z*CubeSize);
            CubeMeshComp->SetWorldLocation(newloc);
            */
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Couldn't load in debug cube material"));
    }

}

void ACubeGenerator::OnConstruction(const FTransform& Transform)
{
    UE_LOG(LogTemp, Warning, TEXT("ACubeGenerator::OnConstruction"));
#if WITH_EDITOR
    GridOrigo = this->GetActorLocation();
    FVector firstCubePos = GridOrigo - ((CubeDimCount/2)*FVector(CubeSize));

    for(size_t i = 0; i < CubeCount; i++)
    {
        auto cube = CubesMeshes[i];
        if(cube != nullptr)
        {
            float x = (float)(i % CubeDimCount);
            float y = (float)((i / CubeDimCount) % CubeDimCount);
            float z = (float)(i / (CubeDimCount*CubeDimCount));
            FVector newloc(firstCubePos.X + x*CubeSize, firstCubePos.Y + y*CubeSize, firstCubePos.Z + z*CubeSize);
            cube->SetWorldLocation(newloc);
        }
    }
#endif

    for(size_t i = 0; i < CubeCount; i++)
    {
        auto cube = CubesMeshes[i];
        if(cube != nullptr)
        {
            cube->mFrequency = mNoiseFrequency;
            cube->mZBias = mZBias;
            cube->GenerateBlock();
        }
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
    TArray<FGridPos> newCubePositions = GatherPositionsForCubes(LocationToGridPos(loc));

    TArray<UTerrainVolumetricChunkComponent*> unusedCubes;
    TArray<FGridPos> unfilledPositions;
    unfilledPositions.Append(newCubePositions);
    
    for(size_t i = 0; i < CubeCount; i++)
    {
        auto cube = CubesMeshes[i];
        FGridPos pos = LocationToGridPos(cube->GetComponentLocation());

        auto stillNeedCubeAtPosition = (newCubePositions.FindByKey(pos) != nullptr);
        if(stillNeedCubeAtPosition)
        {
            unfilledPositions.Remove(pos);
        }
        else
        {
            unusedCubes.Push(cube);
        }
    }

    if(unfilledPositions.Num() > unusedCubes.Num())
        UE_LOG(LogTemp, Warning, TEXT("There are more unfilled positions (%d) than unused cubes(%d)"), unfilledPositions.Num(), unusedCubes.Num());

    for(int i = 0; (i < unfilledPositions.Num()) && (unusedCubes.Num() > 0); i++)
    {
        auto cube = unusedCubes.Pop(false);
        //UE_LOG(LogTemp, Warning, TEXT("Number %d pos: %s"), i, *cubeLocs[i].ToString());
        cube->SetWorldLocation(GridPosToLocation(unfilledPositions[i]));
        cube->GenerateBlock();
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

FGridPos ACubeGenerator::LocationToGridPos(const FVector& loc)
{
    FVector gridLoc = (loc / CubeSize) + FVector(0.5f);
    FGridPos gridPos = FGridPos(FMath::FloorToInt(gridLoc.X), FMath::FloorToInt(gridLoc.Y), FMath::FloorToInt(gridLoc.Z));
    return gridPos;
}

FVector ACubeGenerator::GridPosToLocation(const FGridPos& pos)
{
    FVector result = FVector(pos)*CubeSize;
    return result;
}
