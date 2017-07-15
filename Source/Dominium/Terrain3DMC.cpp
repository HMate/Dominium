// Fill out your copyright notice in the Description page of Project Settings.

#include "Terrain3DMC.h"

#include "MC/marching_cubes.h"

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
    GridZ = TileCountZ + 1;

    NoiseGenerator->SetSeed(mSeed);
    NoiseGenerator->SetFrequency(mFrequency);

    TArray<float> densities;
    TArray<FVector> positions;
    GetDensities(densities, positions);

    TArray<FVector> allVertices;
    PoligoniseChunk(allVertices, densities, positions);


    //GetPositions(vertices, densities, positions);

    TArray<FVector> vertices;
    TArray<int32> indices;
    LoadIndexedVertices(vertices, indices, allVertices);

    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FColor> vertexColors;
    TArray<FRuntimeMeshTangent> tangents;

    UE_LOG(Terrain3DMC, Log, TEXT("UpdatingMesh ---!!!----"));
    MeshGenerator->CreateMeshSection(0, vertices, indices, normals, UV0, vertexColors, tangents, true);
}


void ATerrain3DMC::GetDensities(TArray<float> &Densities, TArray<FVector> Positions)
{
    Densities.Reserve(GridX*GridY*GridZ);
    Positions.Reserve(GridX*GridY*GridZ);
    float tileSizeX = SizeX / (float)(TileCountX);
    float tileSizeY = SizeY / (float)(TileCountY);
    float tileSizeZ = SizeZ / (float)(TileCountZ);

    for(auto zIndex = 0; zIndex < GridZ; zIndex++)
    {
        for(auto yIndex = 0; yIndex < GridY; yIndex++)
        {
            for(auto xIndex = 0; xIndex < GridX; xIndex++)
            {
                float x = tileSizeX*xIndex;
                float y = tileSizeY*yIndex;
                float z = tileSizeZ*zIndex;
                float w = NoiseGenerator->GetGradient(x, y, z);
                Positions.Add(FVector(x, y, z));
                Densities.Add(w);
            }
        }
    }
}

void ATerrain3DMC::PoligoniseChunk(TArray<FVector> &Vertices, const TArray<float> &Densities, const TArray<FVector> &Positions)
{
    Vertices.Reserve(GridX*GridY*GridZ);
    float tileSizeX = SizeX / (float)(TileCountX);
    float tileSizeY = SizeY / (float)(TileCountY);
    float tileSizeZ = SizeZ / (float)(TileCountZ);

    for(auto zIndex = 0; zIndex < GridZ - 1; zIndex++)
    {
        for(auto yIndex = 0; yIndex < GridY - 1; yIndex++)
        {
            for(auto xIndex = 0; xIndex < GridX - 1; xIndex++)
            {
                GRIDCELL cell;
                int32 x0 = xIndex;
                int32 x1 = xIndex + 1;
                int32 y0 = (GridX)*yIndex;
                int32 y1 = (GridX)*(yIndex + 1);
                int32 z0 = (GridX*GridY)*zIndex;
                int32 z1 = (GridX*GridY)*(zIndex + 1);
                cell.p[0] = Positions[z0 + y0 + x0];
                cell.p[1] = Positions[z0 + y0 + x1];
                cell.p[2] = Positions[z0 + y1 + x1];
                cell.p[3] = Positions[z0 + y1 + x0];
                cell.p[4] = Positions[z1 + y0 + x0];
                cell.p[5] = Positions[z1 + y0 + x1];
                cell.p[6] = Positions[z1 + y1 + x1];
                cell.p[7] = Positions[z1 + y1 + x0];
                cell.val[0] = Densities[z0 + y0 + x0];
                cell.val[1] = Densities[z0 + y0 + x1];
                cell.val[2] = Densities[z0 + y1 + x1];
                cell.val[3] = Densities[z0 + y1 + x0];
                cell.val[4] = Densities[z1 + y0 + x0];
                cell.val[5] = Densities[z1 + y0 + x1];
                cell.val[6] = Densities[z1 + y1 + x1];
                cell.val[7] = Densities[z1 + y1 + x0];
                TRIANGLE triangles[5];
                uint32 tcount = Polygonise(cell, 0.0f, triangles);
                for(uint32 t = 0; t < tcount; t++)
                {
                    Vertices.Add(triangles[t].p[0]);
                    Vertices.Add(triangles[t].p[1]);
                    Vertices.Add(triangles[t].p[2]);
                }
            }
        }
    }
}

void ATerrain3DMC::LoadIndexedVertices(TArray<FVector> &Vertices, TArray<int32> &Indices, const TArray<FVector> &AllVertex)
{
    Vertices.Reserve(AllVertex.Num()/2);
    Indices.Reserve(AllVertex.Num());

    for(auto v : AllVertex)
    {
        // OPTIMIZATION: lookup for stored vertices might be faster with TMap?
        auto vindex = Vertices.IndexOfByPredicate([v](auto vx) { 
            return v.Equals(vx); 
        });
        if(vindex == INDEX_NONE)
        {
            vindex = Vertices.Add(v);
        }
        Indices.Add(vindex);
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

