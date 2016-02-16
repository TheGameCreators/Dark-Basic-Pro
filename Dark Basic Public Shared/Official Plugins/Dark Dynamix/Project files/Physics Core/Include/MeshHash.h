#ifndef MESH_HASH_H
#define MESH_HASH_H


#include <NxArray.h>
#include <NxVec3.h>
#include <NxBounds3.h>

namespace SOFTBODY
{

// ------------------------------------------------------------------------------

struct MeshHashRoot
{
	int first;
	int timeStamp;
};

// ------------------------------------------------------------------------------
struct MeshHashEntry
{
	int itemIndex;
	int next;
};

// ------------------------------------------------------------------------------

class MeshHash
{
public:
	MeshHash();
	~MeshHash();

	void   setGridSpacing(NxReal spacing);
	NxReal getGridSpacing() { return 1.0f / mInvSpacing; }
	void reset();
	void add(const NxBounds3 &bounds, int itemIndex);
	void add(const NxVec3 &pos, int itemIndex);

	void query(const NxBounds3 &bounds, NxArray<int> &itemIndices, int maxIndices = -1);
	void queryUnique(const NxBounds3 &bounds, NxArray<int> &itemIndices, int maxIndices = -1);

	void query(const NxVec3 &pos, NxArray<int> &itemIndices, int maxIndices = -1);
	void queryUnique(const NxVec3 &pos, NxArray<int> &itemIndices, int maxIndices = -1);

private:

	void compressIndices(NxArray<int> &itemIndices);

	inline int hashFunction(int xi, int yi, int zi)
  {
    unsigned int h = (xi * 92837111)^(yi * 689287499)^(zi * 283923481);
    return h % mHashIndexSize;
  }

	inline void cellCoordOf(const NxVec3 &v, int &xi, int &yi, int &zi)
  {
		xi = (int)(v.x * mInvSpacing); if (v.x < 0.0f) xi--;
		yi = (int)(v.y * mInvSpacing); if (v.y < 0.0f) yi--;
		zi = (int)(v.z * mInvSpacing); if (v.z < 0.0f) zi--;
	}

	void quickSort(NxArray<int> &itemIndices, int l, int r);

	static const int       mHashIndexSize = 17011;
	NxReal                 mSpacing;
	NxReal                 mInvSpacing;
	int                    mTime;
	MeshHashRoot           mHashIndex[mHashIndexSize];
	NxArray<MeshHashEntry> mEntries;
};


};

#endif
