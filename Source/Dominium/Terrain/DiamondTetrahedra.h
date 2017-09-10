#pragma once

#include "CoreMinimal.h"

struct DiamondLookupIndexEntry
{
    static const int offsetMaxSize = 32;

    FIntVector diamondType;
    FIntVector address;

    int offsetSize;
    FIntVector offsets[DiamondLookupIndexEntry::offsetMaxSize];

public:
    DiamondLookupIndexEntry(const FIntVector& diamondType, const FIntVector& iAddress);

    void AddOffset(const FIntVector& iOffset);
    void AddNormalizedOffset(const FIntVector& iOffset);
    void Merge(const DiamondLookupIndexEntry &iOther);

    bool HasSameDiamondType(const DiamondLookupIndexEntry& other);
    FString DiamondLookupIndexEntry::ToDetailsString();
};

class DiamondTetrahedra
{
    struct Edge
    {
        enum Edgemask
        {
            E01 = 0x01 | 0x02,
            E02 = 0x01 | 0x04,
            E03 = 0x01 | 0x08,
            E12 = 0x02 | 0x04,
            E13 = 0x02 | 0x08,
            E23 = 0x04 | 0x08,
            EFULL = 0x01 | 0x02 | 0x04 | 0x08,
        };

        FVector& v0;
        FVector& v1;
        Edgemask mask;
        Edge(FVector& v0, FVector& v1, Edgemask mask) : v0(v0), v1(v1), mask(mask) {}
    };

    TArray<FVector, FDefaultAllocator> v;
    TArray<FIntVector, FDefaultAllocator> vi;

    DiamondTetrahedra* child0 = nullptr;
    DiamondTetrahedra* child1 = nullptr;

    int32 hLevel;

public:
    DiamondTetrahedra::DiamondTetrahedra();
    DiamondTetrahedra(FVector v0, FVector v1, FVector v2, FVector v3);
    ~DiamondTetrahedra();

    DiamondTetrahedra& SetVertices(int32 iLevel, FVector v0, FVector v1, FVector v2, FVector v3);
    DiamondTetrahedra& Tesselate(TArray<FVector>& oVertices, TArray<int32>& oIndices, const float iSizer=200.0f);

    DiamondTetrahedra::Edge GetEdge(DiamondTetrahedra::Edge::Edgemask iMask);
    DiamondTetrahedra::Edge GetLongestEdge();
    FVector GetCentralVertex();
    FIntVector GetCentralVertexInt();
    DiamondTetrahedra::Edge GetOffSpineEdge();
    FIntVector GetDiamondIndex();
    DiamondLookupIndexEntry GetLookupIndexEntry();

    bool IsCentralVertexOnGrid();
    bool CanBeSplit();
    bool AlreadySplit();
    void Split(DiamondTetrahedra& d0, DiamondTetrahedra& d1);

    FString ToString();
    FString ToDetailsString();
};