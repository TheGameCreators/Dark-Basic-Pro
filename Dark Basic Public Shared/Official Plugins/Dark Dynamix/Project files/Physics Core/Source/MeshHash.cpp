#include "MeshHash.h"


namespace SOFTBODY
{

// -------------------------------------------------------------------------------------
MeshHash::MeshHash()
{
	mTime = 1;
	MeshHashRoot *r = mHashIndex;
	for (int i = 0; i < mHashIndexSize; i++)
  {
		r->first = -1;
		r->timeStamp = 0;
		r++;
	}
	mSpacing = 0.25f;
	mInvSpacing = 1.0f / mSpacing;
}

// -------------------------------------------------------------------------------------
MeshHash::~MeshHash()
{
}

// -------------------------------------------------------------------------------------
void MeshHash::setGridSpacing(NxReal spacing)
{
	mSpacing = spacing;
	mInvSpacing = 1.0f / spacing;
	reset();
}

// -------------------------------------------------------------------------------------
void MeshHash::reset()
{
	mTime++;
	mEntries.clear();
}

// -------------------------------------------------------------------------------------
void MeshHash::add(const NxBounds3 &bounds, int itemIndex)
{
	int x1,y1,z1;
	int x2,y2,z2;
	int x,y,z;
	cellCoordOf(bounds.min, x1,y1,z1);
	cellCoordOf(bounds.max, x2,y2,z2);
	MeshHashEntry entry;
	entry.itemIndex = itemIndex;

	for (x = x1; x <= x2; x++) {
		for (y = y1; y <= y2; y++) {
			for (z = z1; z <= z2; z++) {
				int h = hashFunction(x,y,z);
				MeshHashRoot &r = mHashIndex[h];
				int n = mEntries.size();
				if (r.timeStamp != mTime || r.first < 0)
					entry.next = -1;
				else
					entry.next = r.first;
				r.first = n;
				r.timeStamp = mTime;
				mEntries.push_back(entry);
			}
		}
	}
}

// -------------------------------------------------------------------------------------
void MeshHash::add(const NxVec3 &pos, int itemIndex)
{
	int x,y,z;
	cellCoordOf(pos, x,y,z);
	MeshHashEntry entry;
	entry.itemIndex = itemIndex;

	int h = hashFunction(x,y,z);
	MeshHashRoot &r = mHashIndex[h];
	int n = mEntries.size();
	if (r.timeStamp != mTime || r.first < 0)
		entry.next = -1;
	else
		entry.next = r.first;
	r.first = n;
	r.timeStamp = mTime;
	mEntries.push_back(entry);
}

// ----------------------------------------------------------------------------------------------------------
void MeshHash::query(const NxBounds3 &bounds, NxArray<int> &itemIndices, int maxIndices)
{
	int x1,y1,z1;
	int x2,y2,z2;
	int x,y,z;

	cellCoordOf(bounds.min, x1,y1,z1);
	cellCoordOf(bounds.max, x2,y2,z2);

	itemIndices.clear();

	for (x = x1; x <= x2; x++)
  {
		for (y = y1; y <= y2; y++)
	  {
			for (z = z1; z <= z2; z++)
		  {
				int h = hashFunction(x,y,z);
				MeshHashRoot &r = mHashIndex[h];
				if (r.timeStamp != mTime) continue;
				int i = r.first;
				while (i >= 0)
			  {
					MeshHashEntry &entry = mEntries[i];
					itemIndices.push_back(entry.itemIndex);
					if (maxIndices >= 0 && (int)itemIndices.size() >= maxIndices) return;
					i = entry.next;
				}
			}
		}
	}
}

// -------------------------------------------------------------------------
void MeshHash::quickSort(NxArray<int> &itemIndices, int l, int r)
{
	int i,j, mi;
	int k, m;

	i = l; j = r; mi = (l + r)/2;
	m = itemIndices[mi];
	while (i <= j)
  {
		while(itemIndices[i] < m) i++;
		while(m < itemIndices[j]) j--;
		if (i <= j)
	  {
			k = itemIndices[i]; itemIndices[i] = itemIndices[j]; itemIndices[j] = k;
			i++; j--;
		}
	}
	if (l < j) quickSort(itemIndices, l, j);
	if (i < r) quickSort(itemIndices, i, r);
}

// ----------------------------------------------------------------------------------------------------------
void MeshHash::compressIndices(NxArray<int> &itemIndices)
{
	if (itemIndices.size() == 0) return;
	// sort results
	quickSort(itemIndices, 0, itemIndices.size()-1);
	// mark duplicates
	int i = 0;
	while (i < (int)itemIndices.size())
  {
		int j = i+1;
		while (j < (int)itemIndices.size() && itemIndices[i] == itemIndices[j])
	  {
			itemIndices[j] = -1; j++;
		}
		i = j;
	}
	// remove duplicates
	i = 0;
	while (i < (int)itemIndices.size())
  {
		if (itemIndices[i] < 0)
	  {
			itemIndices[i] = itemIndices[itemIndices.size()-1];
			itemIndices.pop_back();
		}
		else i++;
	}
}

// -------------------------------------------------------------------------------------
void MeshHash::queryUnique(const NxBounds3 &bounds, NxArray<int> &itemIndices, int maxIndices)
{
	query(bounds, itemIndices, maxIndices);
	compressIndices(itemIndices);
}

// ----------------------------------------------------------------------------------------------------------
void MeshHash::query(const NxVec3 &pos, NxArray<int> &itemIndices, int maxIndices)
{
	int x,y,z;
	cellCoordOf(pos, x,y,z);
	itemIndices.clear();

	int h = hashFunction(x,y,z);
	MeshHashRoot &r = mHashIndex[h];
	if (r.timeStamp != mTime) return;
	int i = r.first;
	while (i >= 0)
  {
		MeshHashEntry &entry = mEntries[i];
		itemIndices.push_back(entry.itemIndex);
		if (maxIndices >= 0 && (int)itemIndices.size() >= maxIndices) return;
		i = entry.next;
	}
}

// -------------------------------------------------------------------------------------
void MeshHash::queryUnique(const NxVec3 &pos, NxArray<int> &itemIndices, int maxIndices)
{
	query(pos, itemIndices, maxIndices);
	compressIndices(itemIndices);
}

}; // END OF SOFTBODY NAMESPACE
