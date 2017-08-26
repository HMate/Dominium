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
    MeshIndex = 0;

    float cubeSize = 8.0;
    float sizer = 200.0 / cubeSize;

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
    TArray<int32> indices;

    // v3,v5 is the spine - after that first go left then on right edge
    DiamondTetrahedra t0 = DiamondTetrahedra(v3, v5, v1, v0);
    DiamondTetrahedra t1 = DiamondTetrahedra(v3, v5, v2, v1);
    DiamondTetrahedra t2 = DiamondTetrahedra(v3, v5, v0, v4);
    DiamondTetrahedra t3 = DiamondTetrahedra(v3, v5, v4, v7);
    DiamondTetrahedra t4 = DiamondTetrahedra(v3, v5, v7, v6);
    DiamondTetrahedra t5 = DiamondTetrahedra(v3, v5, v6, v2);

    DiamondTetrahedra t10 = DiamondTetrahedra();
    DiamondTetrahedra t11 = DiamondTetrahedra();
    DiamondTetrahedra t12 = DiamondTetrahedra();
    DiamondTetrahedra t13 = DiamondTetrahedra();
    DiamondTetrahedra t14 = DiamondTetrahedra();
    DiamondTetrahedra t15 = DiamondTetrahedra();
    DiamondTetrahedra t16 = DiamondTetrahedra();
    DiamondTetrahedra t17 = DiamondTetrahedra();
    DiamondTetrahedra t18 = DiamondTetrahedra();
    DiamondTetrahedra t19 = DiamondTetrahedra();
    DiamondTetrahedra t110 = DiamondTetrahedra();
    DiamondTetrahedra t111 = DiamondTetrahedra();

    t0.Split(t10, t11);
    t1.Split(t12, t13);
    t2.Split(t14, t15);
    t3.Split(t16, t17);
    t4.Split(t18, t19);
    t5.Split(t110, t111);

    t10.Tesselate(vertices, indices, sizer);
    t11.Tesselate(vertices, indices, sizer);
    t12.Tesselate(vertices, indices, sizer);
    t13.Tesselate(vertices, indices, sizer);
    t14.Tesselate(vertices, indices, sizer);
    t15.Tesselate(vertices, indices, sizer);
    t16.Tesselate(vertices, indices, sizer);
    t17.Tesselate(vertices, indices, sizer);
    t18.Tesselate(vertices, indices, sizer);
    t19.Tesselate(vertices, indices, sizer);
    t110.Tesselate(vertices, indices, sizer);
    t111.Tesselate(vertices, indices, sizer);

    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 0 center: %s"), *t0.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 1 center: %s"), *t1.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 2 center: %s"), *t2.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 3 center: %s"), *t3.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 4 center: %s"), *t4.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 5 center: %s"), *t5.GetCentralVertice().ToString());

    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 10 center: %s"),  *t10.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 11 center: %s"),  *t11.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 12 center: %s"),  *t12.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 13 center: %s"),  *t13.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 14 center: %s"),  *t14.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 15 center: %s"),  *t15.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 16 center: %s"),  *t16.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 17 center: %s"),  *t17.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 18 center: %s"),  *t18.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 19 center: %s"),  *t19.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 110 center: %s"), *t110.GetCentralVertice().ToString());
    UE_LOG(LogTemp, Log, TEXT("Tetrahedra 111 center: %s"), *t111.GetCentralVertice().ToString());

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

