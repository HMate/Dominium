// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeGenerator.h"

#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"


// Sets default values
ACubeGenerator::ACubeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    SceneComponent->SetWorldLocation(FVector(0.0, 0.0, 0.0));
    RootComponent = SceneComponent;


    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/StarterContent/Props/Materials/M_StatueGlass.M_StatueGlass'"));
    UMaterial* cubeMaterial;

    if(Material.Object != NULL)
    {
        cubeMaterial = (UMaterial*)Material.Object;

        for(int i = 0; i < CubeCount; i++)
        {
            FString compName = FString("CubeMesh") + FString::FromInt(i);
            UStaticMeshComponent* CubeMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(*compName));
            CubeMeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            CubeMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
            CubeMeshComp->SetStaticMesh(CubeMesh);

            CubeMeshComp->SetRelativeScale3D(4.0f*FVector::OneVector);

            //UMaterialInstanceDynamic* cubeMaterialDyn = UMaterialInstanceDynamic::Create(cubeMaterial, this);
            CubeMeshComp->SetMaterial(0, cubeMaterial);

            CubesMeshes.Add(CubeMeshComp);

            const float cubeSize = 400.0f;
            FVector min = (0 - 1.0f*cubeSize)*FVector::OneVector;
            float x = (float)(i % CubeDimCount);
            float y = (float)((i / CubeDimCount) % CubeDimCount);
            float z = (float)(i / (CubeDimCount*CubeDimCount));
            FVector newloc(min.X+x*cubeSize, min.Y + y*cubeSize, min.Z + z*cubeSize);
            //CubeMeshComp->SetWorldLocation(newloc);
            CubeMeshComp->SetRelativeLocation(newloc);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Couldn't load in debug cube material"));
    }

}

// Called when the game starts or when spawned
void ACubeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

