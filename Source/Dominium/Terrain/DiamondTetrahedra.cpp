#include "DiamondTetrahedra.h"


DiamondTetrahedra::DiamondTetrahedra()
{
    child0 = nullptr;
    child1 = nullptr;
}

DiamondTetrahedra::~DiamondTetrahedra()
{
    child0 = nullptr;
    child1 = nullptr;
}

// Order of vertices doesnt matter
DiamondTetrahedra::DiamondTetrahedra(FVector v0, FVector v1, FVector v2, FVector v3) : DiamondTetrahedra()
{
    SetVertices(v0, v1, v2, v3);
}

DiamondTetrahedra& DiamondTetrahedra::SetVertices(FVector v0, FVector v1, FVector v2, FVector v3)
{
    v.Empty(4);
    vi.Empty(4);

    v.Add(v0);
    v.Add(v1);
    v.Add(v2);
    v.Add(v3);

    vi.Emplace(v0.X, v0.Y, v0.Z);
    vi.Emplace(v1.X, v1.Y, v1.Z);
    vi.Emplace(v2.X, v2.Y, v2.Z);
    vi.Emplace(v3.X, v3.Y, v3.Z);

    return *this;
}

DiamondTetrahedra& DiamondTetrahedra::Tesselate(TArray<FVector>& oVertices, TArray<int32>& oIndices, const float iSizer)
{
    if(v.Num() < 4)
        return *this;

    if(child0 != nullptr && child1 != nullptr)
    {
        child0->Tesselate(oVertices, oIndices, iSizer);
        child1->Tesselate(oVertices, oIndices, iSizer);
        return *this;
    }

    int32 vindex0 = oVertices.Emplace(v[0]* iSizer);
    int32 vindex1 = oVertices.Emplace(v[1]* iSizer);
    int32 vindex2 = oVertices.Emplace(v[2]* iSizer);
    int32 vindex3 = oVertices.Emplace(v[3]* iSizer);

    oIndices.Add(vindex0);
    oIndices.Add(vindex1);
    oIndices.Add(vindex2);

    oIndices.Add(vindex2);
    oIndices.Add(vindex1);
    oIndices.Add(vindex3);

    oIndices.Add(vindex3);
    oIndices.Add(vindex1);
    oIndices.Add(vindex0);

    oIndices.Add(vindex3);
    oIndices.Add(vindex0);
    oIndices.Add(vindex2);

    return *this;
}

DiamondTetrahedra::Edge DiamondTetrahedra::GetLongestEdge()
{
    TArray<DiamondTetrahedra::Edge> edges;
    edges.Emplace(GetEdge(Edge::E01));
    edges.Emplace(GetEdge(Edge::E02));
    edges.Emplace(GetEdge(Edge::E03));
    edges.Emplace(GetEdge(Edge::E12));
    edges.Emplace(GetEdge(Edge::E13));
    edges.Emplace(GetEdge(Edge::E23));

    int longestEdgeIndex = 0;
    float max = (edges[0].v0 - edges[0].v1).SizeSquared();
    for(size_t i=1; i<edges.Num(); i++)
    {
        auto& e = edges[i];
        float length = (e.v0 - e.v1).SizeSquared();
        if(length > max)
        {
            max = length;
            longestEdgeIndex = i;
        }
    }
    return edges[longestEdgeIndex];
}

DiamondTetrahedra::Edge DiamondTetrahedra::GetOffSpineEdge()
{
    DiamondTetrahedra::Edge e = GetLongestEdge();
    Edge::Edgemask offmask = Edge::Edgemask(Edge::EFULL ^ e.mask);
    return GetEdge(offmask);
}

DiamondTetrahedra::Edge DiamondTetrahedra::GetEdge(DiamondTetrahedra::Edge::Edgemask iMask)
{
    switch(iMask)
    {
    case DiamondTetrahedra::Edge::E01:
        return Edge(v[0], v[1], Edge::E01);
    case DiamondTetrahedra::Edge::E02:
        return Edge(v[0], v[2], Edge::E02);
    case DiamondTetrahedra::Edge::E03:
        return Edge(v[0], v[3], Edge::E03);
    case DiamondTetrahedra::Edge::E12:
        return Edge(v[1], v[2], Edge::E12);
    case DiamondTetrahedra::Edge::E13:
        return Edge(v[1], v[3], Edge::E13);
    case DiamondTetrahedra::Edge::E23:
        return Edge(v[2], v[3], Edge::E23);
    default:
        checkf(false, TEXT("DiamondTetrahedra::GetEdge got unknown edge mask: %d"), iMask);
        return Edge(v[2], v[3], Edge::E23);
    }
}

bool DiamondTetrahedra::CanBeSplit()
{
    // If central vx is not on lattice, we cant split it
    FVector c = GetCentralVertex();
    FVector truncated = FVector(FIntVector(c.X, c.Y, c.Z));
    if(!c.Equals(truncated))
        return false;

    // If already splitted, dont split it
    if(child0 != nullptr && child1 != nullptr)
        return false;

    // TODO: Really it can only be splitted if every tetrahedra in its the diamond is can be split
    // if(MyDiamond->HasEveryTetrahedra()) is true
    return true;
}

void DiamondTetrahedra::Split(DiamondTetrahedra& d0, DiamondTetrahedra& d1)
{
    child0 = nullptr;
    child1 = nullptr;

    FVector c = GetCentralVertex();

    DiamondTetrahedra::Edge e = GetLongestEdge();
    DiamondTetrahedra::Edge o = GetOffSpineEdge();

    d0.SetVertices(c, e.v0, o.v0, o.v1);
    d1.SetVertices(c, e.v1, o.v0, o.v1);

    child0 = &d0;
    child1 = &d1;
}

FVector DiamondTetrahedra::GetCentralVertex()
{
    DiamondTetrahedra::Edge e = GetLongestEdge();
    FVector cv = e.v0 + ((e.v1 - e.v0) / 2.0f);
    return cv;
}


