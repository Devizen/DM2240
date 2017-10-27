#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include "Vector3.h"
#include <list>
class CSpatialPartition
{
public:
	CSpatialPartition(void);
	CSpatialPartition(unsigned _partitionNum);
	CSpatialPartition(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
	~CSpatialPartition(void);
	/*Setters*/
	void SetPosition(Vector3 _position) { spPosition = _position; }
	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }
	/*Set the partition number the player is in.*/
	void SetPartition(unsigned _partitionNum) { partitionNum.push_back(_partitionNum); }
	void ClearPartition() { partitionNum.clear(); }

	/*Getters*/
	Vector3 GetPosition(void) { return spPosition; }
	Vector3 GetMinBoundary(void) { return minBoundary; }
	Vector3 GetMaxBoundary(void) { return maxBoundary; }
	/*Get the partition number the player is in.*/
	std::list<unsigned int> GetPartition(void) { return partitionNum; }

	bool CheckBoundary(Vector3 playerPosition);
private:
	/*Value to remember which partition the player is in.*/
	std::list<unsigned int> partitionNum;
	Vector3 spPosition;
	Vector3 minBoundary;
	Vector3 maxBoundary;
//	CSpatialPartition(void);
//	CSpatialPartition(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
//	~CSpatialPartition(void);
//	/*Setters*/
//	void SetPosition(Vector3 _position) { position = _position; }
//	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
//	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }
//
//	/*Getters*/
//	Vector3 GetPosition(void) { return position; }
//	Vector3 GetMinBoundary(void) { return minBoundary; }
//	Vector3 GetMaxBoundary(void) { return maxBoundary; }
//private:
//	Vector3 position;
//	Vector3 minBoundary;
//	Vector3 maxBoundary;
protected:

};

#endif