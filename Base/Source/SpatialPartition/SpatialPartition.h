#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include "Vector3.h"

class CSpatialPartition
{
public:
	CSpatialPartition(void);
	CSpatialPartition(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
	~CSpatialPartition(void);
	/*Setters*/
	void SetPosition(Vector3 _position) { position = _position; }
	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }

	/*Getters*/
	Vector3 GetPosition(void) { return position; }
	Vector3 GetMinBoundary(void) { return minBoundary; }
	Vector3 GetMaxBoundary(void) { return maxBoundary; }
private:
	Vector3 position;
	Vector3 minBoundary;
	Vector3 maxBoundary;
protected:

};

#endif