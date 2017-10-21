#include "PartitionInfo.h"

CPartitionInfo::CPartitionInfo(void)
{
}

CPartitionInfo::CPartitionInfo(unsigned _partitionNum)
	: partitionNum(_partitionNum)
	, position(0.f, 0.f, 0.f)
	, minBoundary(0.f, 0.f, 0.f)
	, maxBoundary(0.f, 0.f, 0.f)
{
}

CPartitionInfo::~CPartitionInfo(void)
{
}

bool CPartitionInfo::CheckBoundary(Vector3 playerPosition)
{
	//if (playerPosition < minBoundary && playerPosition < maxBoundary ||
	//	playerPosition > minBoundary && playerPosition > maxBoundary)
	//	return true;

	//return false;
	if (playerPosition >= minBoundary &&
		playerPosition <= maxBoundary)
		return false;
	else
		return true;
}
