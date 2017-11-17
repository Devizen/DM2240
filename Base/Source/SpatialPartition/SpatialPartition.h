#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include "Vector3.h"
#include <list>
class CSpatialPartition
{
public:
	CSpatialPartition(void);
	CSpatialPartition(unsigned _partitionNum);
	CSpatialPartition(unsigned _index, Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
	~CSpatialPartition(void);
	/*Setters*/
	void SetIndex(unsigned _index) { index = _index; }
	//void SetPosition(Vector3 _position) { position = _position; }
	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }

	/*Getters*/
	unsigned GetIndex(void) { return index; }
	//Vector3 GetPosition(void) { return position; }
	Vector3 GetMinBoundary(void) { return minBoundary; }
	Vector3 GetMaxBoundary(void) { return maxBoundary; }
	void SetPosition(Vector3 _position) { spPosition = _position; }
	/*Set the partition number the player is in.*/
	void SetPartition(unsigned _partitionNum) { partitionNum.push_back(_partitionNum); }
	/*Clears current partition list and reassign a new one*/
	void SetPartition(std::list<unsigned>& _partitionList) { partitionNum.assign(_partitionList.begin(), _partitionList.end()); }
	void ClearPartition() { partitionNum.clear(); }

	/*Getters*/
	Vector3 GetPosition(void) { return spPosition; }
	//Vector3 GetMinBoundary(void) { return minBoundary; }
	//Vector3 GetMaxBoundary(void) { return maxBoundary; }
	/*Get the partition number the player is in.*/
	std::list<unsigned int> GetPartition(void) { return partitionNum; }
	std::list<unsigned int>* GetPartitionPtr(void) { return &partitionNum; }

	/*Object's position should be passed in, if function returned true, proceed to run SpatialPartitionManager UpdateGridInfo().*/
	bool CheckBoundary(Vector3 objectPosition);
private:
	unsigned index;
	//Vector3 position;
	/*Value to remember which partition the player is in.*/
	std::list<unsigned int> partitionNum;
	Vector3 spPosition;
	Vector3 minBoundary;
	Vector3 maxBoundary;
protected:

};

#endif