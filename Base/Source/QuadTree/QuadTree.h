#ifndef QUADTREE_H
#define QUADTREE_H

/*For positions.*/
#include "../../Common/Source/Vector3.h"
#include <list>
#include <vector>

class EntityBase;
class QuadTreeManager;
class QuadTree
{
	friend QuadTreeManager;
public:
	/*Only use X and Y or X and Z. 2D.
	Width and Length of QuadTree can be calculated via _minBoundary and _maxBoundary.*/
	QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary, unsigned _level, unsigned _maxLevel, std::list<EntityBase*> entityList);
	~QuadTree(void);

	bool CheckGrid(std::list<Vector3>positionList);
	/*Split grid into four.*/
	void Split(void);

	void Render(void);
	void Update(double dt);
	void DeleteChildren();
	std::vector<QuadTree*> GetAllChildren();
	Vector3 GetPosition();
	Vector3 GetMinBoundary();
	Vector3 GetMaxBoundary();

	void SetRenderGrid(bool render) { this->RenderGrid = render; }
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

	/*Size of Quad.*/
	float quadSize;

	/*For expanding range.*/
	QuadTree* parent;
	/*Entities that are in the quad.*/
	std::list<EntityBase*>entityList;

	unsigned level;
	unsigned maxLevel;

	bool RenderGrid;
protected:
};

#endif