// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UnrealFastNoisePlugin/Public/FastNoise/FastNoise.h"
#include "RuntimeMeshComponent.h"

#include "Terrain2D.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Terrain2D, Log, All);


UCLASS()
class DOMINIUM_API ATerrain2D : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrain2D();

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
	int TileCountX = 100;
	UPROPERTY(EditAnywhere, Category = Chunk) // Specifies how many tiles does a chunk consist of
	int TileCountY = 100;

protected:

    // How many samples do we take in this chunk. Always should be true: GridN == TileCountN+1
	int GridX;
	int GridY;

	FVector LastCameraPos;

	UPROPERTY(VisibleAnywhere, Category = Generator)
	URuntimeMeshComponent *MeshGenerator;
	UPROPERTY(VisibleAnywhere, Category = Generator)
	UFastNoise *NoiseGenerator;

protected:

	void GenerateBlock(int32 pBlockIndex);
	void UpdateBlock(int32 pBlockIndex);
	void GetPositions(TArray<FVector> &Positions);
	void LoadIndices(TArray<int32> &Indices);

	virtual void OnConstruction(const FTransform& Transform) override;

	
	
};
