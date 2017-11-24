#ifndef QUADTREE_H
#define QUADTREE_H

/*For positions.*/
#include "Vector3.h"
#include <list>

class EntityBase;

class QuadTree
{
public:
	/*Only use X and Y or X and Z. 2D.
	Width and Length of QuadTree can be calculated via _minBoundary and _maxBoundary.*/
	QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary, unsigned _level, unsigned _maxLevel, std::list<EntityBase*>_entityList);
	~QuadTree(void);

	bool CheckGrid(std::list<Vector3>positionList);
	/*Split grid into four.*/
	void Split(void);

	void Render(void);
	void Update(double dt);

private:
	QuadTree* topLeft;
	QuadTree* topRight;
	QuadTree* bottomLeft;
	QuadTree* bottomRight;

	/*Boundary of grid.*/
	Vector3 minBoundary;
	Vector3 maxBoundary;

	/*Position of grid.*/
	Vector3 position;

	/*Width of Grid.*/
	float width;
	/*Length of Grid.*/
	float length;
	/*For expanding range.*/
	QuadTree* parent;
	/*Entities that are in the quad.*/
	std::list<EntityBase*>entityList;

	unsigned level;
	unsigned maxLevel;
protected:
};

#endif