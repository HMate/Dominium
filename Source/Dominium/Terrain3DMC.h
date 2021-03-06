// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UnrealFastNoisePlugin/Public/FastNoise/FastNoise.h"
#include "RuntimeMeshComponent.h"

// .generated.h needs to be the last include in the header!
#include "Terrain3DMC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Terrain3DMC, Log, All);

UCLASS()
class DOMINIUM_API ATerrain3DMC : public AActor
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = Noise) // The Random seed
    int mSeed = 22;

    UPROPERTY(EditAnywhere, Category = Noise) // Frequency of noise
    float mFrequency = 0.01f;

    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeX = 1000.0f;
    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeY = 1000.0f;
    UPROPERTY(EditAnywhere, Category = Chunk) // Size of terrain chunk in world
    float SizeZ = 1000.0f;

    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountX = 15;
    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountY = 15;
    UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
    int TileCountZ = 15;

protected:
    // How many samples do we take in this chunk. Always should be true: GridN == TileCountN+1
    int GridX;
    int GridY;
    int GridZ;

    UPROPERTY(VisibleAnywhere, Category = Generator)
    URuntimeMeshComponent *MeshGenerator;
    UPROPERTY(VisibleAnywhere, Category = Generator)
    UFastNoise *NoiseGenerator;

public:
    // Sets default values for this actor's properties
    ATerrain3DMC();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    void GenerateBlock(int32 pBlockIndex);
    void UpdateBlock(int32 pBlockIndex);
    void GetDensities(TArray<float> &Densities, TArray<FVector> &Positions);
    void PoligoniseChunk(TArray<FVector> &Vertices, const TArray<float> &Densities, const TArray<FVector> &Positions);
    void LoadIndexedVertices(TArray<FVector> &Vertices, TArray<int32> &Indices, const TArray<FVector> &AllVertex);

};
