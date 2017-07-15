// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UnrealFastNoisePlugin/Public/FastNoise/FastNoise.h"
#include "RuntimeMeshComponent.h"

#include "Terrain3DMC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Terrain3DMC, Log, All);

UCLASS()
class DOMINIUM_API ATerrain3DMC : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrain3DMC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = Noise) // The Random seed
    int mSeed = 22;

    UPROPERTY(EditAnywhere, Category = Noise) // Frequency of noise
    float mFrequency = 0.01f;

    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeX = 100.0f;
    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeY = 100.0f;
    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeZ = 100.0f;

    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountX = 8;
    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountY = 8;
    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountZ = 8;

protected:

    // How many samples do we take in this chunk. Always should be true: GridN == TileCountN+1
    int GridX;
    int GridY;
    int GridZ;

    FVector LastCameraPos;

    UPROPERTY(VisibleAnywhere, Category = Generator)
    URuntimeMeshComponent *MeshGenerator;
    UPROPERTY(VisibleAnywhere, Category = Generator)
    UFastNoise *NoiseGenerator;

protected:

    void GenerateBlock(int32 pBlockIndex);
    void UpdateBlock(int32 pBlockIndex);
    void GetDensities(TArray<float> &Densities, TArray<FVector> &Positions);
    void PoligoniseChunk(TArray<FVector> &Vertices, const TArray<float> &Densities, const TArray<FVector> &Positions);
    void LoadIndexedVertices(TArray<FVector> &Vertices, TArray<int32> &Indices, const TArray<FVector> &AllVertex);

    virtual void OnConstruction(const FTransform& Transform) override;

	
	
};
