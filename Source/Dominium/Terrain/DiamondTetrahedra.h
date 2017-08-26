#pragma once

#include "CoreMinimal.h"

class DiamondTetrahedra
{
    struct Edge
    {
        FVector& v0;
        FVector& v1;
        Edge(FVector& v0, FVector& v1) : v0(v0), v1(v1) {}
    };

    TArray<FVector, FDefaultAllocator> v;
    TArray<FIntVector, FDefaultAllocator> vi;

public:
    DiamondTetrahedra::DiamondTetrahedra();
    DiamondTetrahedra(FVector v0, FVector v1, FVector v2, FVector v3);

    DiamondTetrahedra& SetVertices(FVector v0, FVector v1, FVector v2, FVector v3);
    DiamondTetrahedra& Tesselate(TArray<FVector>& oVertices, TArray<int32>& oIndices, const float iSizer=200.0f);

    DiamondTetrahedra::Edge DiamondTetrahedra::GetLongestEdge();
    FVector DiamondTetrahedra::GetCentralVertice();
    DiamondTetrahedra::Edge GetOffSpineEdge();
    void Split(DiamondTetrahedra& d0, DiamondTetrahedra& d1);
};