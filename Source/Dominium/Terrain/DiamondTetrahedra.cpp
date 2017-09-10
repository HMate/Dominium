#include "DiamondTetrahedra.h"


DiamondTetrahedra::DiamondTetrahedra()
{
    child0 = nullptr;
    child1 = nullptr;
    hLevel = -1;
}

DiamondTetrahedra::~DiamondTetrahedra()
{
    child0 = nullptr;
    child1 = nullptr;
}

// Order of vertices doesnt matter
DiamondTetrahedra::DiamondTetrahedra(FVector v0, FVector v1, FVector v2, FVector v3) : DiamondTetrahedra()
{
    SetVertices(0, v0, v1, v2, v3);
}

DiamondTetrahedra& DiamondTetrahedra::SetVertices(int32 iLevel, FVector v0, FVector v1, FVector v2, FVector v3)
{
    hLevel = iLevel;
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

FIntVector ToIntVector(FVector v)
{
    return FIntVector(v.X, v.Y, v.Z);
}

bool DiamondTetrahedra::CanBeSplit()
{
    // If central vx is not on lattice, we cant split it
    bool isGrid = IsCentralVertexOnGrid();

    // TODO: Really it can only be splitted if every tetrahedra in its the diamond is can be split
    // if(MyDiamond->HasEveryTetrahedra()) is true
    return isGrid;
}

bool DiamondTetrahedra::IsCentralVertexOnGrid()
{
    FVector c = GetCentralVertex();
    FVector truncated = FVector(ToIntVector(c));
    if(!c.Equals(truncated))
        return false;
    return true;
}

bool DiamondTetrahedra::AlreadySplit()
{
    if(child0 != nullptr && child1 != nullptr)
        return true;
    return false;
}

void DiamondTetrahedra::Split(DiamondTetrahedra& d0, DiamondTetrahedra& d1)
{
    child0 = nullptr;
    child1 = nullptr;

    FVector c = GetCentralVertex();

    DiamondTetrahedra::Edge e = GetLongestEdge();
    DiamondTetrahedra::Edge o = GetOffSpineEdge();

    d0.SetVertices(hLevel+1, c, e.v0, o.v0, o.v1);
    d1.SetVertices(hLevel+1, c, e.v1, o.v0, o.v1);

    child0 = &d0;
    child1 = &d1;
}

FVector DiamondTetrahedra::GetCentralVertex()
{
    DiamondTetrahedra::Edge e = GetLongestEdge();
    FVector cv = e.v0 + ((e.v1 - e.v0) / 2.0f);
    return cv;
}

FIntVector DiamondTetrahedra::GetCentralVertexInt()
{
    FVector cv = GetCentralVertex();
    FIntVector cvi = ToIntVector(cv);
    return cvi;
}

FString DiamondTetrahedra::ToString()
{
    return FString::Printf(TEXT("Tetrhdr Pos((%s) (%s) (%s) (%s))"), *vi[0].ToString(), *vi[1].ToString(), *vi[2].ToString(), *vi[3].ToString());
}

FString printBits(int32 ptr)
{
    unsigned char *b = (unsigned char*)&ptr;
    unsigned char byte;
    FString result;

    for(int i = 3; i>=0; i--)
    {
        for(int j = 7; j >= 0; j--)
        {
            byte = (b[i] >> j) & 1;
            result += FString::Printf(TEXT("%u"), byte);
        }
    }
    return result;
}

// rightmost set bit index
int rsb(int x)
{
    return (x != 0) ? FMath::FloorLog2(x & -x) : 32;
}

// is sigma th bit unset
int ibu(int x, int sigma)
{
    return (1 ^ ((x >> sigma) & 1));
}

//diamondIndex: can be used as an index in the diamond hierarchy for the diamond vertices
FIntVector DiamondTetrahedra::GetDiamondIndex()
{
    auto cvi = GetCentralVertexInt();
    // sigma: index of rightmost 1 in byte representation of center
    // also gives the depth level of the diamond in the heirarchy
    int sigma = FMath::Min3(rsb(cvi.X), rsb(cvi.Y), rsb(cvi.Z));
    FIntVector diamondIndex = FIntVector(((cvi.X >> sigma) & 3), ((cvi.Y >> sigma) & 3), ((cvi.Z >> sigma) & 3));
    return diamondIndex;
}

FString DiamondTetrahedra::ToDetailsString()
{
    auto cvi = GetCentralVertexInt();
    // sigma: index of rightmost 1 in byte representation of center
    // also gives the depth level of the diamond in the heirarchy
    int sigma = FMath::Min3(rsb(cvi.X), rsb(cvi.Y), rsb(cvi.Z));
    // diamondType: {0, 1, 2}, number of unset bits at sigmath bit of every coordinate
    int diamondType = ibu(cvi.X, sigma) + ibu(cvi.Y, sigma) + ibu(cvi.Z, sigma);
    FIntVector theta = GetDiamondIndex();
    return FString::Printf(TEXT("Level %d\nx %s sigma: %d\ny %s class: %d\nz %s theta: %s)"), 
        hLevel, *printBits(cvi.X), sigma, *printBits(cvi.Y), diamondType, *printBits(cvi.Z), *theta.ToString());
}

DiamondLookupIndexEntry DiamondTetrahedra::GetLookupIndexEntry()
{
    auto center = GetCentralVertexInt();
    DiamondLookupIndexEntry entry = DiamondLookupIndexEntry(GetDiamondIndex(), center);

    for(size_t i = 0; i < vi.Num(); i++)
    {
        auto off = vi[i] - center;
        entry.AddNormalizedOffset(off);
    }
    return entry;
}


// --------------------------------------------------------------------------------------------------------------------------------


DiamondLookupIndexEntry::DiamondLookupIndexEntry(const FIntVector& diamondType, const FIntVector& iAddress)
    : diamondType(diamondType), address(iAddress), offsetSize(0)
{
}

bool DiamondLookupIndexEntry::HasSameDiamondType(const DiamondLookupIndexEntry& other)
{
    return diamondType == other.diamondType;
}

void DiamondLookupIndexEntry::AddNormalizedOffset(const FIntVector& iOffset)
{
    FIntVector norm = iOffset;
    norm.X = FMath::Clamp(norm.X, -1, 1);
    norm.Y = FMath::Clamp(norm.Y, -1, 1);
    norm.Z = FMath::Clamp(norm.Z, -1, 1);
    AddOffset(norm);
}

void DiamondLookupIndexEntry::AddOffset(const FIntVector& iOffset)
{
    if(offsetSize < DiamondLookupIndexEntry::offsetMaxSize)
    {
        offsets[offsetSize] = iOffset;
        offsetSize++;
    }
    //else
    //{
    //    checkf(false, TEXT("DiamondLookupIndexEntry offset array too small"));
    //}
}

void DiamondLookupIndexEntry::Merge(const DiamondLookupIndexEntry &iOther)
{
    for(size_t i = 0; i < iOther.offsetSize; i++)
    {
        bool has = false;
        for(size_t j = 0; j < offsetSize; j++)
        {
            if(offsets[j] == iOther.offsets[i])
                has = true;
        }
        if(!has)
            AddOffset(iOther.offsets[i]);
    }
}

FString DiamondLookupIndexEntry::ToDetailsString()
{
    FString offsetString;
    for(size_t i = 0; i < offsetSize; i++)
    {
        offsetString += FString::Printf(TEXT("\n (%s)"), *offsets[i].ToString());
    }
    return FString::Printf(TEXT("Type: %s { %s }"),
        *diamondType.ToString(), *offsetString);
}
