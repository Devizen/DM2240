//#ifndef PARTITIONINFO_H
//#define PARTITIONINFO_H
//
//#include "Vector3.h"
//
//class CPartitionInfo
//{
//public:
//	CPartitionInfo(void);
//	CPartitionInfo(unsigned _partitionNum);
//	~CPartitionInfo(void);
//	/*Setters*/
//	void SetPosition(Vector3 _position) { position = _position; }
//	void SetMinBoundary(Vector3 _minBoundary) { minBoundary = _minBoundary; }
//	void SetMaxBoundary(Vector3 _maxBoundary) { maxBoundary = _maxBoundary; }
//	/*Set the partition number the player is in.*/
//	void SetPartition(unsigned _partitionNum) { partitionNum = _partitionNum; }
//
//	/*Getters*/
//	Vector3 GetPosition(void) { return position; }
//	Vector3 GetMinBoundary(void) { return minBoundary; }
//	Vector3 GetMaxBoundary(void) { return maxBoundary; }
//	/*Get the partition number the player is in.*/
//	unsigned GetPartition(void) { return partitionNum; }
//
//	bool CheckBoundary(Vector3 playerPosition);
//private:
//	/*Value to remember which partition the player is in.*/
//	unsigned partitionNum;
//	Vector3 position;
//	Vector3 minBoundary;
//	Vector3 maxBoundary;
//protected:
//};
//
//#endif