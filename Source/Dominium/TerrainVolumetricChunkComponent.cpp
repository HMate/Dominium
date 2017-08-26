// Fill out your copyright notice in the Description page of Project Settings.

#include "TerrainVolumetricChunkComponent.h"

#include "Terrain/marching_cubes.h"


// Sets default values for this component's properties
UTerrainVolumetricChunkComponent::UTerrainVolumetricChunkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    MeshGenerator = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh"));
    MeshGenerator->bTickInEditor = true;

    /*static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Dominium/DebugCubeMaterial.DebugCubeMaterial'"));
    UMaterial* cubeMaterial;

    if(Material.Object != NULL)
    {
        cubeMaterial = (UMaterial*)Material.Object;
        MeshGenerator->SetMaterial(0, cubeMaterial);
    }*/

    NoiseGenerator = CreateDefaultSubobject<UFastNoise>(TEXT("NoiseGenerator"));
    NoiseGenerator->SetNoiseType(ENoiseType::Gradient);
    NoiseGenerator->SetFractalType(EFractalType::FBM);
    NoiseGenerator->SetFractalOctaves(5);

    GridX = TileCountX + 1;
    GridY = TileCountY + 1;
    GridZ = TileCountZ + 1;
}

void UTerrainVolumetricChunkComponent::GenerateBlock()
{
    NoiseGenerator->SetSeed(mSeed);
    NoiseGenerator->SetFrequency(mFrequency);

    TArray<float> densities;
    TArray<FVector> positions;
    GetDensities(densities, positions);

    TArray<FVector> allVertices;
    PoligoniseChunk(allVertices, densities, positions);

    TArray<FVector> vertices;
    TArray<int32> indices;
    LoadIndexedVertices(vertices, indices, allVertices);

    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FColor> vertexColors;
    TArray<FRuntimeMeshTangent> tangents;

    //UE_LOG(Terrain3DMC, Log, TEXT("UpdatingMesh ---!!!----"));
    if(vertices.Num() > 0)
    {
        MeshGenerator->CreateMeshSection(0, vertices, indices, normals, UV0, vertexColors, tangents, true);
        //MeshGenerator->DoesSectionExist(0);
    }
    else
    {
        MeshGenerator->ClearMeshSection(0);
    }
}

// Negative density is air, positive is ground
void UTerrainVolumetricChunkComponent::GetDensities(TArray<float> &Densities, TArray<FVector> &Positions)
{
    FVector volumePosition = GetComponentLocation();
    FVector boundsMin = volumePosition - (0.5f*FVector(SizeX, SizeY, SizeZ));

    Densities.Reserve(GridX*GridY*GridZ);
    Positions.Reserve(GridX*GridY*GridZ);
    float tileSizeX = SizeX / (float)(TileCountX);
    float tileSizeY = SizeY / (float)(TileCountY);
    float tileSizeZ = SizeZ / (float)(TileCountZ);

    float min = 100.0, max = -100.0;

    for(auto zIndex = 0; zIndex < GridZ; zIndex++)
    {
        for(auto yIndex = 0; yIndex < GridY; yIndex++)
        {
            for(auto xIndex = 0; xIndex < GridX; xIndex++)
            {
                float x = boundsMin.X + tileSizeX*xIndex;
                float y = boundsMin.Y + tileSizeY*yIndex;
                float z = boundsMin.Z + tileSizeZ*zIndex;
                float w0 = NoiseGenerator->GetGradientFractal(x, y, z);
                min = ((min < w0) ? min : w0);
                max = ((max > w0) ? max : w0);
                float w = (-mZBias*z) + w0;

                Positions.Add(FVector(x, y, z));
                Densities.Add(w);
            }
        }
    }
    //UE_LOG(LogTemp, Log, TEXT("tile pos %s -- min: %f max: %f"), *boundsMin.ToString(), min, max);
}

void UTerrainVolumetricChunkComponent::PoligoniseChunk(TArray<FVector> &Vertices, const TArray<float> &Densities, const TArray<FVector> &Positions)
{
    Vertices.Reserve(GridX*GridY*GridZ);
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
                // Polygonise creates cw triangles, and assumes positive isovalues are air
                // We need ccw triangles, and positive is ground -> kinda like double negate, so they will be good for us as is.
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

void UTerrainVolumetricChunkComponent::LoadIndexedVertices(TArray<FVector> &Vertices, TArray<int32> &Indices, const TArray<FVector> &AllVertex)
{
    Vertices.Reserve(AllVertex.Num() / 2);
    Indices.Reserve(AllVertex.Num());

    for(auto v : AllVertex)
    {
        // OPTIMIZATION: lookup for stored vertices might be faster with TMap?
        auto vindex = Vertices.IndexOfByPredicate([&v](auto vx) {
            return v.Equals(vx);
        });
        if(vindex == INDEX_NONE)
        {
            vindex = Vertices.Add(v);
        }
        Indices.Add(vindex);
    }
}

// Called when the game starts
void UTerrainVolumetricChunkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTerrainVolumetricChunkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UTerrainVolumetricChunkComponent::GetSize()
{
    return FVector(SizeX, SizeY, SizeZ);
}