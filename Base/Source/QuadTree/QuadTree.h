#ifndef QUADTREE_H
#define QUADTREE_H

/*For positions.*/
#include "Vector3.h"
#include <list>

class QuadTree
{
public:
	QuadTree(void);
	QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary);
	~QuadTree(void);

	bool CheckGrid(std::list<Vector3>positionList);
	/*Split grid into four.*/
	void Split(void);

private:
	QuadTree* childOne;
	QuadTree* childTwo;
	QuadTree* childThree;
	QuadTree* childFour;

	/*Boundary of grid.*/
	Vector3 minBoundary;
	Vector3 maxBoundary;

	/*Position of grid.*/
	Vector3 position;

	/*Width of Grid.*/
	float width;
	/*Length of Grid.*/
	float length;

	QuadTree* parent;
protected:
};

#endif