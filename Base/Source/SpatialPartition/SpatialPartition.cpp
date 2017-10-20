#include "SpatialPartition.h"

CSpatialPartition::CSpatialPartition(void)
{
}

CSpatialPartition::CSpatialPartition(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary)
	: position(_position)
	, minBoundary(_minBoundary)
	, maxBoundary(_maxBoundary)
{
}

CSpatialPartition::~CSpatialPartition(void)
{
}
