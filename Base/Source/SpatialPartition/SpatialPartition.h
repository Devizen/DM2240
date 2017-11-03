#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include "Vector3.h"

class CSpatialPartition
{
public:
	CSpatialPartition(void);
	CSpatialPartition(unsigned _partitionNum);
	CSpatialPartition(unsigned _index, Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
	~CSpatialPartition(void);
	/*Setters*/
	void SetIndex(unsigned _index) { index = _index; }
	void SetPosition(Vector3 _position) { position = _position; }
	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }

	/*Getters*/
	unsigned GetIndex(void) { return index; }
	Vector3 GetPosition(void) { return position; }
	Vector3 GetMinBoundary(void) { return minBoundary; }
	Vector3 GetMaxBoundary(void) { return maxBoundary; }

	/*Object's position should be passed in, if function returned true, proceed to run SpatialPartitionManager UpdateGridInfo().*/
	bool CheckBoundary(Vector3 objectPosition);
private:
	unsigned index;
	Vector3 position;
	Vector3 minBoundary;
	Vector3 maxBoundary;
protected:

};

#endif