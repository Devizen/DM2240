#include "QuadTree.h"

QuadTree::QuadTree(void)
	: parent(nullptr)
	, childOne(nullptr)
	, childTwo(nullptr)
	, childThree(nullptr)
	, childFour(nullptr)
	, minBoundary(0.f, 0.f, 0.f)
	, maxBoundary(0.f ,0.f, 0.f)
	, position(0.f, 0.f, 0.f)
	, width(0.f)
	, length(0.f)
{
}

QuadTree::QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary)
	: position(_position)
	, minBoundary(_minBoundary)
	, maxBoundary(_maxBoundary)
{
	parent = nullptr;
	childOne = nullptr;
	childTwo = nullptr;
	childThree = nullptr;
	childFour = nullptr;

	width = (_maxBoundary.x - _minBoundary.x) >= (_maxBoundary.z - _minBoundary.z) ?
		(_maxBoundary.z - _minBoundary.z) : (_maxBoundary.x - _minBoundary.x);

	length = (_maxBoundary.x - _minBoundary.x) <= (_maxBoundary.z - _minBoundary.z) ?
		(_maxBoundary.z - _minBoundary.z) : (_maxBoundary.x - _minBoundary.x);
}

QuadTree::~QuadTree(void)
{
}

bool QuadTree::CheckGrid(std::list<Vector3> positionList)
{
	for (std::list<Vector3>::iterator it = positionList.begin(); it != positionList.end(); ++it)
	{
		Vector3 entity = *it;
		
	}
	return false;
}

void QuadTree::Split(void)
{
	
}
