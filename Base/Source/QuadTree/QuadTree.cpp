#include "QuadTree.h"

#include "QuadTreeManager.h"
#include "EntityBase.h"
#include "CameraManager.h"
#include "Collider\Collider.h"
#include "../GenericEntity.h"

QuadTree::QuadTree(Vector3 _position, Vector3 _minBoundary, Vector3 _maxBoundary, unsigned _level, unsigned _maxLevel, std::list<EntityBase*> entityList)
	: position(_position)
	, minBoundary(_minBoundary)
	, maxBoundary(_maxBoundary)
	, level(_level)
	, maxLevel(_maxLevel)
	, entityList(entityList)
	, topLeft(nullptr)
	, topRight(nullptr)
	, bottomLeft(nullptr)
	, bottomRight(nullptr)
	, RenderGrid(false)
	, parent(nullptr)
{
	//if (level == maxLevel)
	//	return;

	/*Does not matter if we use x axis or z axis as it will always be the same value.
	For example, minBoundary (0.f, 0.f, 0.f) and maxBoundary (400.f, 0.f, 400.f).
	Another example, minBoundary (200.f, 0.f, 200.f) and maxBoundary (400.f, 0.f, 400.f).
	Another another example, minBoundary (200.f, 0.f, 0.f) and maxBoundary (400.f, 0.f, 200.f).*/
	quadSize = _maxBoundary.x - _minBoundary.x;
	unsigned count = 0;

	/*Need to check how many objects in grid.
	For example,
	
	for (std::list<EntityBase*>::iterator it = _entityList.begin(); it != _entityList.end(); ++it) {
		if (CheckInside((*it)))
			++count;
	
	}
	*/
	
	/*if (count > 4) {
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
	}*/

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
	//DeleteChildren();
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
	if (level + 1 > maxLevel)
		return;
	//aiya shouldve used a vector instead of just floats
	/*Theory example:
	maxBoundary = 400.f
	minBoundary = 0.f

	width = 400.f - 0.f = 400.f
	height = 400.f - 0.f = 400.f
	halfX = (400.f * 0.5f) + 0.f = 200.f
	halfY = (400.f * 0.5f) + 0.f = 200.f


	 400 -  -  -  -
		 |    |   |
	 200 |____|___|
		 |    |   |
		 |    |   |
		 -  -  -  -
		0    200   400

	Position for topLeftX = (200.f - 400.f) * 0.25f = -50.f
	Position for topLeftY = (200.f + 400.f) * 0.25f = 150.f
	Min Boundary = (0.f, 200.f, 0.f)
	Max Boundary = (200.f, 400.f, 0.f)
	*/
	//float width = (this->maxBoundary.x - this->minBoundary.x);
	//float height = (this->maxBoundary.y - this->minBoundary.y);
	//float halfX = width * 0.5f + this->minBoundary.x;
	//float halfY = height * 0.5f + this->minBoundary.y;

	//topLeft = new QuadTree(Vector3(halfX - width * 0.25f, halfY + height * 0.25f, 0),
	//	Vector3(minBoundary.x, halfY, 0),
	//	Vector3(halfX, maxBoundary.y, 0), level + 1, maxLevel, this->entityList);

	//topRight = new QuadTree(Vector3(halfX + width * 0.25f, halfY + height * 0.25f, 0), Vector3(halfX, halfY, 0),
	//	maxBoundary, level + 1, maxLevel, this->entityList);

	//bottomLeft = new QuadTree(Vector3(halfX - width * 0.25f, halfY - height * 0.25f, 0), minBoundary,
	//	Vector3(halfX, halfY), level + 1, maxLevel, this->entityList);

	//bottomRight = new QuadTree(Vector3(halfX + width * 0.25f, halfY - height * 0.25f, 0), Vector3(halfX, minBoundary.y, 0),
	//	Vector3(maxBoundary.x, halfY), level + 1, maxLevel, this->entityList);




	/*FULLY OPTIMSIED FOR ALL TYPE OF QUAD SIZES.*/

	/*My suggested changes, always start the quad at bottom left position, this allows us to use Position and Min Boundary as the same value.*/
	/*I am using X and Z because we are not drawing the Quad to the sky. It is front, back, left and right.*/
	/*I initialise the Vector3 first so that I don't have to create the memory for 2 Vector3 for Position and Min Boundary.*/
	Vector3 _topLeft(position.x, position.y, position.z + (quadSize * 0.5f));
	Vector3 _topRight(position.x + (quadSize * 0.5f), position.y, position.z + (quadSize * 0.5f));
	Vector3 _bottomLeft(position.x, position.y, position.z);
	Vector3 _bottomRight(position.x + (quadSize * 0.5f), position.y, position.z);




	/*Assuming QuadTree started creating at Vector3(0.f, 0.f, 0.f) and maxBoundary (400.f, 0.f, 400.f)*/

	/*topLeft will be created at (0.f, 0.f, 200.f), minBoundary = (0.f, 0.f, 200.f) and maxBoundary = (200.f, 0.f, 400.f).*/
	topLeft = new QuadTree(_topLeft, Vector3(_topLeft.x, -1000, _topLeft.z), Vector3(_topLeft.x + (quadSize * 0.5f), position.y + 1000, _topLeft.z + (quadSize * 0.5f)), level + 1, maxLevel, {});
	QuadTreeManager::GetInstance()->PutEntitiesInGrid(topLeft, entityList);
	/*topRight will be created at (200.f, 0.f, 200.f), minBoundary = (200.f, 0.f, 200.f) and maxBoundary = (400.f, 0.f, 400.f).*/
	topRight = new QuadTree(_topRight, Vector3(_topRight.x, -1000, _topRight.z), Vector3(_topRight.x + (quadSize * 0.5f), position.y + 1000, _topRight.z + (quadSize * 0.5f)), level + 1, maxLevel, {});
	QuadTreeManager::GetInstance()->PutEntitiesInGrid(topRight, entityList);
	/*bottomLeft will be created at (0.f, 0.f, 0.f), minBoundary = (0.f, 0.f, 0.f) and maxBoundary = (200.f, 0.f, 200.f).*/
	bottomLeft = new QuadTree(_bottomLeft, Vector3(_bottomLeft.x, -1000, _bottomLeft.z), Vector3(_bottomLeft.x + (quadSize * 0.5f), position.y + 1000, _bottomLeft.z + (quadSize * 0.5f)), level + 1, maxLevel, {});
	QuadTreeManager::GetInstance()->PutEntitiesInGrid(bottomLeft, entityList);
	/*bottomRight will be created at (200.f, 0.f, 0.f), minBoundary = (200.f, 0.f, 0.f) and maxBoundary = (400.f, 0.f, 200.f).*/
	bottomRight = new QuadTree(_bottomRight, Vector3(_bottomRight.x, -1000, _bottomRight.z), Vector3(_bottomRight.x + (quadSize * 0.5f), position.y + 1000, _bottomRight.z + (quadSize * 0.5f)), level + 1, maxLevel, {});
	QuadTreeManager::GetInstance()->PutEntitiesInGrid(bottomRight, entityList);

	//put parent
	topLeft->parent = this;
	topRight->parent = this;
	bottomLeft->parent = this;
	bottomRight->parent = this;
}

//#include "MeshBuilder.h"
//#include "GraphicsManager.h"
//#include "RenderHelper.h"
//#include "Mesh.h"
void QuadTree::Render(void)
{

	//Mesh* quad = MeshBuilder::GetInstance()->GetMesh("W_GRASS");

	//quad->material.kAmbient.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));

	//MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	//ms.PushMatrix();
	//ms.Rotate(-90, 1, 0, 0);
	//ms.Translate(this->position);
	////ms.Translate(this->maxBoundary - this->minBoundary * 0.5f);
	//ms.Translate(0, 5, 0);
	//ms.Rotate(90, 0, 0, 1);
	//ms.Scale(this->maxBoundary - this->minBoundary);
	//RenderHelper::RenderMesh(quad);
	//ms.PopMatrix();

	
}

void QuadTree::Update(double dt)
{

	FPSCamera* cam = CameraManager::GetInstance()->GetPlayerCam();

	for (auto e : entityList) {

		if ((e->GetPosition() - cam->GetCameraPos()).LengthSquared() < 100 * 100)
			e->GetLoD().SetLevel(CLevelOfDetail::HIGH);
		else if ((e->GetPosition() - cam->GetCameraPos()).LengthSquared() < 200 * 200)
			e->GetLoD().SetLevel(CLevelOfDetail::MEDIUM);
		else
			e->GetLoD().SetLevel(CLevelOfDetail::LOW);
		
		//e->Update(dt);

		if (this->RenderGrid == false) {
			GenericEntity* ge = dynamic_cast<GenericEntity*>(e);
			if (ge)
			{
				RenderGrid = CameraManager::GetInstance()->IsAABBInFrustum(ge->collider->GetMinAABB() , ge->collider->GetMaxAABB());
			}

			//Vector3 min = e->collider->GetMinAABB();
			//Vector3 max = e->collider->GetMaxAABB();
			//Vector3 boundingPoints[8];
			//boundingPoints[0] = Vector3(min);
			//boundingPoints[1] = Vector3(min.x, min.y, max.z);
			//boundingPoints[2] = Vector3(max.x, min.y, max.z);
			//boundingPoints[3] = Vector3(max.x, min.y, min.z);

			//boundingPoints[4] = Vector3(min.x, max.y, min.z);
			//boundingPoints[5] = Vector3(min.x, max.y, max.z);
			//boundingPoints[6] = Vector3(max.x, max.y, max.z);
			//boundingPoints[7] = Vector3(max.x, max.y, min.z);

			////check if any points is in withing 180 of cam
			//Vector3 camRight = (cam->GetCameraTarget() - cam->GetCameraPos()).Cross(cam->GetCameraUp()).Normalize();
			//Vector3 camDir = (cam->GetCameraTarget() - cam->GetCameraPos()).Normalized();
			//for (int i = 0; i < 8; ++i) {

			//	if (Math::RadianToDegree(acos(camDir.Dot((boundingPoints[i] - cam->GetCameraPos()).Normalized()))) < 90.f) {
			//		this->RenderGrid = true;
			//		break;
			//	}
			//}
		}
	
		
	}


}

void QuadTree::DeleteChildren()
{
	if (topLeft)
	{
		topLeft->DeleteChildren();
		delete topLeft;
	}
	if (topRight)
	{
		topRight->DeleteChildren();
		delete topRight;
	}
	if (bottomLeft)
	{
		bottomLeft-> DeleteChildren();
		delete bottomLeft;
	}
	if (bottomRight)
	{
		bottomRight->DeleteChildren();
		delete bottomRight;
	}
}

std::vector<QuadTree*> QuadTree::GetAllChildren()
{
	if (topLeft == nullptr)
		return std::vector<QuadTree*>();

	return std::vector<QuadTree*> {topLeft, topRight, bottomLeft, bottomRight};
}

Vector3 QuadTree::GetPosition()
{
	return this->position;
}

Vector3 QuadTree::GetMinBoundary()
{
	return this->minBoundary;
}

Vector3 QuadTree::GetMaxBoundary()
{
	return this->maxBoundary;
}
