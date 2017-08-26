// Fill out your copyright notice in the Description page of Project Settings.

#include "DiamondHierarchyTester.h"

//#include "Debug"

// Sets default values
ADiamondHierarchyTester::ADiamondHierarchyTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshGenerator = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = MeshGenerator;
    
    UMaterial* cubeMaterial = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/Dominium/DebugCubeMaterial.DebugCubeMaterial'")).Get();
    
    if(cubeMaterial != NULL)
    {
        for(int i = 0; i < 6; i++)
        {
            MeshGenerator->SetMaterial(i, cubeMaterial);
        }
    }
}

void ADiamondHierarchyTester::OnConstruction(const FTransform& Transform)
{
    MeshGenerator->ClearAllMeshSections();
    m_MeshIndex = 0;
    m_TetrahedraStore.Empty(MaxTetrahedraCount);

    float cubeSize = (float)FMath::Min(64, FMath::Max(1, (1 << FMath::FloorLog2(HierarchyMaxLevel))));
    float sizer = CubeWorldSize / cubeSize;

    //cube looks like this:
    //       7-------6     z^  ^
    //      /|      /|      | /x
    //     / |     / |      |/
    //    4-------5  |      ------>
    //    |  3----|--2          y
    //    | /     | /
    //    0-------1
    FVector v0 = FVector(0.0f, 0.0f, 0.0f) * cubeSize;
    FVector v1 = FVector(0.0f, 1.0f, 0.0f) * cubeSize;
    FVector v2 = FVector(1.0f, 1.0f, 0.0f) * cubeSize;
    FVector v3 = FVector(1.0f, 0.0f, 0.0f) * cubeSize;
    FVector v4 = FVector(0.0f, 0.0f, 1.0f) * cubeSize;
    FVector v5 = FVector(0.0f, 1.0f, 1.0f) * cubeSize;
    FVector v6 = FVector(1.0f, 1.0f, 1.0f) * cubeSize;
    FVector v7 = FVector(1.0f, 0.0f, 1.0f) * cubeSize;

    TArray<FVector> vertices;
    vertices.Empty(1000);
    TArray<int32> indices;
    vertices.Empty(1000);

    // v3,v5 is the spine - after that first go left then on right edge
    m_TetrahedraStore.Emplace(v3, v5, v1, v0);
    m_TetrahedraStore.Emplace(v3, v5, v2, v1);
    m_TetrahedraStore.Emplace(v3, v5, v0, v4);
    m_TetrahedraStore.Emplace(v3, v5, v4, v7);
    m_TetrahedraStore.Emplace(v3, v5, v7, v6);
    m_TetrahedraStore.Emplace(v3, v5, v6, v2);

    for(size_t i = 0; i < m_TetrahedraStore.Num(); i++)
    {
        auto& t = m_TetrahedraStore[i];
        if(t.CanBeSplit() && ((m_TetrahedraStore.Num() + 2) < MaxTetrahedraCount))
        {
            int t0 = m_TetrahedraStore.Emplace();
            int t1 = m_TetrahedraStore.Emplace();
            t.Split(m_TetrahedraStore[t0], m_TetrahedraStore[t1]);
        }
        else if((m_TetrahedraStore.Num() + 2) >= MaxTetrahedraCount)
        {
            UE_LOG(LogTemp, Warning, TEXT("Max Tetrahedra count too low !"));
            break;
        }
        UE_LOG(LogTemp, Log, TEXT("Tetrahedra %d center: %s"), i, *t.GetCentralVertex().ToString());
    }

    for(size_t i = 0; i < 6; i++)
    {
        m_TetrahedraStore[i].Tesselate(vertices, indices, sizer);
    }

    TArray<FVector> normals;
    TArray<FVector2D> UV0;
    TArray<FColor> vertexColors;
    TArray<FRuntimeMeshTangent> tangents;

    if(vertices.Num() > 0)
    {
        MeshGenerator->CreateMeshSection(m_MeshIndex, vertices, indices, normals, UV0, vertexColors, tangents, true);
        m_MeshIndex++;
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

