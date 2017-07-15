// Fill out your copyright notice in the Description page of Project Settings.

#include "Terrain3DMC.h"

DEFINE_LOG_CATEGORY(Terrain3DMC);

// Sets default values
ATerrain3DMC::ATerrain3DMC()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshGenerator = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh"));
    MeshGenerator->bTickInEditor = true;
    RootComponent = MeshGenerator;

    NoiseGenerator = CreateDefaultSubobject<UFastNoise>(TEXT("NoiseGenerator"));
    NoiseGenerator->SetNoiseType(ENoiseType::Gradient);
}

void ATerrain3DMC::OnConstruction(const FTransform& Transform)
{
    GridX = TileCountX + 1;
    GridY = TileCountY + 1;

    NoiseGenerator->SetSeed(mSeed);
    NoiseGenerator->SetFrequency(mFrequency);

    TArray<FVector> vertices;
    GetPositions(vertices);

    TArray<int32> Triangles;
    LoadIndices(Triangles);

    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FColor> vertexColors;
    TArray<FRuntimeMeshTangent> tangents;

    UE_LOG(Terrain3DMC, Log, TEXT("UpdatingMesh ---!!!----"));
    MeshGenerator->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
}


void ATerrain3DMC::GetPositions(TArray<FVector> &Positions)
{
    Positions.Reserve(GridX*GridY);
    float tileSizeX = SizeX / (float)(TileCountX);
    float tileSizeY = SizeY / (float)(TileCountY);
    
    for (int32 yIndex = 0; yIndex < GridY; yIndex++)
    {
        for (int32 xIndex = 0; xIndex < GridX; xIndex++)
        {
            float x = tileSizeX*xIndex;
            float y = tileSizeY*yIndex;
            float z = NoiseGenerator->GetGradient(x, y)* SizeZ;
            Positions.Add(FVector(x, y, z));
        }
    }
}

void ATerrain3DMC::LoadIndices(TArray<int32> &Indices)
{
    Indices.Reserve(GridX*GridY * 4);
    for (int32 y = 0; y < GridY - 1; y++)
    {
        for (int32 x = 0; x < GridX - 1; x++)
        {
            int32 y0Index = y*GridX;
            int32 y1Index = (y + 1)*GridX;
            Indices.Add(y0Index + x);
            Indices.Add(y1Index + x);
            Indices.Add(y0Index + x + 1);
            Indices.Add(y0Index + x + 1);
            Indices.Add(y1Index + x);
            Indices.Add(y1Index + x + 1);
        }
    }
}

// Called when the game starts or when spawned
void ATerrain3DMC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerrain3DMC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

