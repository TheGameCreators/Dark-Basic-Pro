#ifndef MESH_HASH_H
#define MESH_HASH_H

#include "NxPhysics.h"
#include <vector>

// ------------------------------------------------------------------------------

struct MeshHashRoot {
	int first;
	int timeStamp;
};

// ------------------------------------------------------------------------------
struct MeshHashEntry {
	int itemIndex;
	int next;
};

// ------------------------------------------------------------------------------

class MeshHash {
public: 
	MeshHash();
	~MeshHash();

	void setGridSpacing(float spacing);
	NxF32 getGridSpacing() { return 1.0f / mInvSpacing; }
	void reset();
	void add(const NxBounds3 &bounds, int itemIndex);
	void add(const NxVec3 &pos, int itemIndex);

	void query(const NxBounds3 &bounds, std::vector<int> &itemIndices, int maxIndices = -1);
	void queryUnique(const NxBounds3 &bounds, std::vector<int> &itemIndices, int maxIndices = -1);

	void query(const NxVec3 &pos, std::vector<int> &itemIndices, int maxIndices = -1);
	void queryUnique(const NxVec3 &pos, std::vector<int> &itemIndices, int maxIndices = -1);

private:
	void compressIndices(std::vector<int> &itemIndices);
	static const int mHashIndexSize = 17011;
	NxF32 mSpacing;
	NxF32 mInvSpacing;
	int mTime;

	inline int hashFunction(int xi, int yi, int zi) { 
	    unsigned int h = (xi * 92837111)^(yi * 689287499)^(zi * 283923481);
	    return h % mHashIndexSize;
    }

	inline void cellCoordOf(const NxVec3 &v, int &xi, int &yi, int &zi) {
		xi = (int)(v.x * mInvSpacing); if (v.x < 0.0f) xi--;
		yi = (int)(v.y * mInvSpacing); if (v.y < 0.0f) yi--;
		zi = (int)(v.z * mInvSpacing); if (v.z < 0.0f) zi--;
	}

	void quickSort(std::vector<int> &itemIndices, int l, int r);

	MeshHashRoot mHashIndex[mHashIndexSize];
	std::vector<MeshHashEntry> mEntries;
};


#endif
