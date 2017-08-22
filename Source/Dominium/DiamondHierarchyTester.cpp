// Fill out your copyright notice in the Description page of Project Settings.

#include "DiamondHierarchyTester.h"


// Sets default values
ADiamondHierarchyTester::ADiamondHierarchyTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshGenerator = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = MeshGenerator;

    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Dominium/DebugCubeMaterial.DebugCubeMaterial'"));
    UMaterial* cubeMaterial;

    if(Material.Object != NULL)
    {
        cubeMaterial = (UMaterial*)Material.Object;
        for(int i=0; i < 6; i++)
            MeshGenerator->SetMaterial(i, cubeMaterial);
    }
}

void ADiamondHierarchyTester::OnConstruction(const FTransform& Transform)
{
    MeshGenerator->ClearAllMeshSections();
    MeshIndex = 0;

    FVector v0 = FVector(0.0f, 0.0f, 0.0f);
    FVector v1 = FVector(0.0f, 1.0f, 0.0f);
    FVector v2 = FVector(1.0f, 1.0f, 0.0f);
    FVector v3 = FVector(1.0f, 0.0f, 0.0f);
    FVector v4 = FVector(0.0f, 0.0f, 1.0f);
    FVector v5 = FVector(0.0f, 1.0f, 1.0f);
    FVector v6 = FVector(1.0f, 1.0f, 1.0f);
    FVector v7 = FVector(1.0f, 0.0f, 1.0f);

    AddTetrahedra(v3, v5, v1, v0);
    AddTetrahedra(v3, v5, v2, v1);
    AddTetrahedra(v3, v5, v0, v4);
    AddTetrahedra(v3, v5, v4, v7);
    AddTetrahedra(v3, v5, v7, v6);
    AddTetrahedra(v3, v5, v6, v2);

    //UE_LOG(LogTemp, Log, TEXT("Updating ADiamondHierarchyTester ---!!!----"));
}


void ADiamondHierarchyTester::AddTetrahedra(const FVector& v0, const FVector& v1, const FVector& v2, const FVector& v3)
{
    TArray<FVector> vertices;
    TArray<int32> indices;

    vertices.Reserve(100);
    indices.Reserve(100);

    float sizer = 200.0f;
    int32 vindex0 = vertices.Emplace(v0*sizer);
    int32 vindex1 = vertices.Emplace(v1*sizer);
    int32 vindex2 = vertices.Emplace(v2*sizer);
    int32 vindex3 = vertices.Emplace(v3*sizer);

    indices.Add(vindex0);
    indices.Add(vindex1);
    indices.Add(vindex2);

    indices.Add(vindex2);
    indices.Add(vindex1);
    indices.Add(vindex3);

    indices.Add(vindex3);
    indices.Add(vindex1);
    indices.Add(vindex0);

    indices.Add(vindex3);
    indices.Add(vindex0);
    indices.Add(vindex2);

    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FColor> vertexColors;
    TArray<FRuntimeMeshTangent> tangents;

    if(vertices.Num() > 0)
    {
        MeshGenerator->CreateMeshSection(MeshIndex, vertices, indices, normals, UV0, vertexColors, tangents, true);
        MeshIndex++;
    }
}

// Called when the game starts or when spawned
void ADiamondHierarchyTester::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADiamondHierarchyTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

