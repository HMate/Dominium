// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorGenerator.generated.h"

UCLASS()
class DOMINIUM_API AFloorGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorGenerator();
    virtual void OnConstruction(const FTransform& Transform);

    UPROPERTY(VisibleAnywhere, Category = Generator)
    UStaticMeshComponent *FloorMesh;
    UPROPERTY(VisibleAnywhere, Category = Generator)
    UStaticMeshComponent *FloorMesh2;
    UPROPERTY(EditAnywhere, Category = Generator)
    UStaticMesh * FloorMeshAsset;

protected:

    UStaticMeshComponent *ActiveFloorMesh;
    UStaticMeshComponent *PassiveFloorMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    bool IsOverlapping(const FVector& iPos, const FVector& iBounds, const FVector& iTargetPoint);
	
};
