// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RuntimeMeshComponent.h"
#include "Terrain/DiamondTetrahedra.h"

#include "DiamondHierarchyTester.generated.h"


UCLASS()
class DOMINIUM_API ADiamondHierarchyTester : public AActor
{
	GENERATED_BODY()
	
public:	

    UPROPERTY(EditAnywhere, Category = Generator)
    URuntimeMeshComponent *MeshGenerator;

protected:
    int32 MeshIndex;

public:
    // Sets default values for this actor's properties
    ADiamondHierarchyTester();
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

};
