// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain/TerrainGrid.h"
#include "TerrainVolumetricChunkComponent.h"

#include "CubeGenerator.generated.h"



UCLASS()
class DOMINIUM_API ACubeGenerator : public AActor
{
	GENERATED_BODY()
	
public:

    int CubeDimCount = 5;
    int CubeCount = CubeDimCount*CubeDimCount*CubeDimCount;

    float CubeSize = 400.0f;


    UPROPERTY(EditAnywhere, Category = Noise)
    float mNoiseFrequency = 0.001f;
    UPROPERTY(EditAnywhere, Category = Noise)
    float mZBias = 0.001f;

protected:
    TArray<UTerrainVolumetricChunkComponent *> CubesMeshes;
    FVector GridOrigo;

public:
    // Sets default values for this actor's properties
    ACubeGenerator();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform);

    FVector GridPosToLocation(const FGridPos& pos);
    FGridPos LocationToGridPos(const FVector& center);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    TArray<FGridPos> GatherPositionsForCubes(const FGridPos& centerPos);
	
};
