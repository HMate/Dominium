#include "DiamondTetrahedra.h"


DiamondTetrahedra::DiamondTetrahedra()
{}

// Order of vertices doesnt matter
DiamondTetrahedra::DiamondTetrahedra(FVector v0, FVector v1, FVector v2, FVector v3)
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
    edges.Emplace(v[0], v[1]);
    edges.Emplace(v[0], v[2]);
    edges.Emplace(v[0], v[3]);
    edges.Emplace(v[1], v[2]);
    edges.Emplace(v[1], v[3]);
    edges.Emplace(v[2], v[3]);

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

FVector DiamondTetrahedra::GetCentralVertice()
{
    DiamondTetrahedra::Edge e = GetLongestEdge();
    FVector cv = e.v0 + ((e.v1 - e.v0)/2.0f);
    return cv;
}

DiamondTetrahedra::Edge DiamondTetrahedra::GetOffSpineEdge()
{
    DiamondTetrahedra::Edge e = GetLongestEdge();
    int o0 = INDEX_NONE;
    int o1 = INDEX_NONE;
    for(size_t i = 0; i < v.Num(); i++)
    {
        if((v[i] != e.v0) && (v[i] != e.v1)) {
            if(o0 == INDEX_NONE)
                o0 = i;
            else
                o1 = i;
        }
    }

    return DiamondTetrahedra::Edge(v[o0], v[o1]);
}

void DiamondTetrahedra::Split(DiamondTetrahedra& d0, DiamondTetrahedra& d1)
{

    DiamondTetrahedra::Edge e = GetLongestEdge();
    DiamondTetrahedra::Edge o = GetOffSpineEdge();
    FVector c = GetCentralVertice();

    d0.SetVertices(c, e.v0, o.v0, o.v1);
    d1.SetVertices(c, e.v1, o.v0, o.v1);
}



