// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain/TerrainGrid.h"

#include "CubeGenerator.generated.h"



UCLASS()
class DOMINIUM_API ACubeGenerator : public AActor
{
	GENERATED_BODY()
	
public:
    const int CubeDimCount = 3;
    const int CubeCount = CubeDimCount*CubeDimCount*CubeDimCount;
    const float CubeSize = 400.0f;

protected:
    TArray<UStaticMeshComponent *> CubesMeshes;
    FVector GridOrigo;

public:
    // Sets default values for this actor's properties
    ACubeGenerator();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    FGridPos LocationToGridPos(const FVector& center);
    TArray<FVector> GatherLocationsForCubes(const FGridPos& centerPos);
	
};
