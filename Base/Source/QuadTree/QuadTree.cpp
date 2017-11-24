#include "QuadTree.h"

QuadTree::QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary, unsigned _level, unsigned _maxLevel, std::list<EntityBase*>_entityList)
	: position(_position)
	, minBoundary(_minBoundary)
	, maxBoundary(_maxBoundary)
	, level(_level)
	, maxLevel(_maxLevel)
	, entityList(_entityList)
{
	if (level == maxLevel)
		return;

	length = _maxBoundary.x;
	width = _maxBoundary.z;
	unsigned count = 0;
	/*Need to check how many objects in grid.
	For example,
	
	for (std::list<EntityBase*>::iterator it = _entityList.begin(); it != _entityList.end(); ++it) {
		if (CheckInside((*it)))
			++count;
	
	}
	*/
	
	if (count > 4) {
		Vector3 halfValue(_maxBoundary * 0.25f);
		Vector3 _topLeft = Vector3(_position.x - halfValue.x, _position.y, _position.z + halfValue.z);
		Vector3 _topRight = Vector3(_position.x + halfValue.x, _position.y, _position.z + halfValue.z);
		Vector3 _bottomLeft = Vector3(_position.x - halfValue.x, _position.y, _position.z - halfValue.z);
		Vector3 _bottomRight = Vector3(_position.x + halfValue.x, _position.y, _position.z - halfValue.z);

		topLeft = new QuadTree(_topLeft,
			Vector3(_topLeft.x - halfValue.x, _topLeft.y, _topLeft.z - halfValue.z),
			Vector3(_topLeft.x + halfValue.x, _topLeft.y, _topLeft.z + halfValue.z), _level + 1, _maxLevel, _entityList);

		topRight = new QuadTree(_topRight,
			Vector3(_topRight.x - halfValue.x, _topRight.y, _topRight.z - halfValue.z),
			Vector3(_topRight.x + halfValue.x, _topRight.y, _topRight.z + halfValue.z), _level + 1, _maxLevel, _entityList);

		bottomLeft = new QuadTree(_bottomLeft,
			Vector3(_bottomLeft.x - halfValue.x, _bottomLeft.y, _bottomLeft.z - halfValue.z),
			Vector3(_bottomLeft.x + halfValue.x, _bottomLeft.y, _bottomLeft.z + halfValue.z), _level + 1, _maxLevel, _entityList);

		bottomRight = new QuadTree(_bottomRight,
			Vector3(_bottomRight.x - halfValue.x, _bottomRight.y, _bottomRight.z - halfValue.z),
			Vector3(_bottomRight.x + halfValue.x, _bottomRight.y, _bottomRight.z + halfValue.z), _level + 1, _maxLevel, _entityList);
	}

	/*parent = nullptr;
	childOne = nullptr;
	childTwo = nullptr;
	childThree = nullptr;
	childFour = nullptr;

	width = (_maxBoundary.x - _minBoundary.x) >= (_maxBoundary.z - _minBoundary.z) ?
		(_maxBoundary.z - _minBoundary.z) : (_maxBoundary.x - _minBoundary.x);

	length = (_maxBoundary.x - _minBoundary.x) <= (_maxBoundary.z - _minBoundary.z) ?
		(_maxBoundary.z - _minBoundary.z) : (_maxBoundary.x - _minBoundary.x);*/
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

void QuadTree::Render(void)
{
}

void QuadTree::Update(double dt)
{
}
