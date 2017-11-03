#include "SpatialPartition.h"

CSpatialPartition::CSpatialPartition(void)
{
}

CSpatialPartition::CSpatialPartition(unsigned _partitionNum)
	//: 
	: partitionNum(_partitionNum)
	, spPosition(0.f, 0.f, 0.f)
	, minBoundary(0.f, 0.f, 0.f)
	, maxBoundary(0.f, 0.f, 0.f)
{
	partitionNum = {};
}

CSpatialPartition::CSpatialPartition(unsigned _index, Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary)
	: index(_index)
	, spPosition(_position)
	, minBoundary(_minBoundary)
	, maxBoundary(_maxBoundary)
{
}
//CSpatialPartition::CSpatialPartition(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary)
//	: spPosition(_position)
//	, minBoundary(_minBoundary)
//	, maxBoundary(_maxBoundary)
//{
//}

CSpatialPartition::~CSpatialPartition(void)
{
}

bool CSpatialPartition::CheckBoundary(Vector3 objectPosition)
{
	if (objectPosition >= minBoundary &&
		objectPosition <= maxBoundary)
		return false;
	else
		return true;
}