#include "Explosive.h"

#include "../SpatialPartition/SpatialPartitionManager.h"
#include "Manager\CollisionManager.h"
#include "../QuadTree/QuadTreeManager.h"
#include "Collider\Collider.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../EntityTypes/EntityTypes.h"
#include "MeshBuilder.h"

CExplosive::CExplosive(void) : GenericEntity(nullptr)
, m_bStatus(false)
, theDirection(0, 0, 0)
, m_fLifetime(-1.0f)
, m_fSpeed(10.0f)
, theSource(NULL)
{
}

CExplosive::CExplosive(Mesh * _modelMesh) : GenericEntity(_modelMesh)
, m_bStatus(false)
, theDirection(0, 0, 0)
, m_fLifetime(-1)
, m_fSpeed(10.0f)
, theSource(NULL)
{
}

CExplosive::~CExplosive(void)
{
}

void CExplosive::Set(Vector3 theNewPosition, Vector3 theNewDirection, const float m_fLifetime, const float m_fSpeed, const float mass)
{
	position = theNewPosition;
	theDirection = theNewDirection;
	this->m_fLifetime = m_fLifetime;
	this->m_fSpeed = m_fSpeed;
	this->mass = mass;

	this->SetPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(theNewPosition)->GetIndex());

	//For normal collision
	this->collider = new CCollider(this);
	this->collider->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
	CollisionManager::GetInstance()->AddCollider(this->collider, this->GetPartitionPtr());

	//for legit collision
	QuadTreeManager::GetInstance()->InsertEntity(this);
	this->SetAABB(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	this->m_bStatus = true;
	
	this->SetIsParent(true);
	CSceneNode* rootNodeForCube = Create::SceneNode(this, this, this);
	/*Create an empty Scene Graph for aCube which will be rendered and updated in QuadTreeManager.
	The entire Scene Graph will be traversed from the root.*/
	this->SetSceneGraph(Create::SceneGraph(rootNodeForCube));
	//Create::SceneNode(rootNodeForCube, rootNodeForCube, aCube)

	//CSceneNode* rootNodeForCube = Create::SceneNode(nullptr, nullptr, nullptr);
	///*Create an empty Scene Graph for aCube which will be rendered and updated in QuadTreeManager.
	//The entire Scene Graph will be traversed from the root.*/
	//this->SetSceneGraph(Create::SceneGraph(rootNodeForCube));
	//CSceneNode* aCubeNode = Create::SceneNode(rootNodeForCube, rootNodeForCube, this);
}

void CExplosive::Update(double dt)
{
	if (m_bStatus == false)
		return;

	// Update Position
	position.Set(position.x + (float)(theDirection.x * dt * m_fSpeed),
		position.y + (float)(theDirection.y * dt * m_fSpeed),
		position.z + (float)(theDirection.z * dt * m_fSpeed));

	this->ClearPartition();
	if (CSpatialPartitionManager::GetInstance()->UpdateGridInfo(position))
		this->SetPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(position)->GetIndex());

}

void CExplosive::Render(void)
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	if (!rotateAxis.IsZero())
		modelStack.Rotate(rotateAngle, rotateAxis.x, rotateAxis.y, rotateAxis.z);
	//modelStack.Scale(scale.x, scale.y, scale.z);
	RenderHelper::RenderMesh(modelMesh);
	modelStack.PopMatrix();
}

void CExplosive::CollisionResponse(EntityBase * other)
{
	//std::cout << "BOOM : " << other << std::endl;
	//isDone = true;
}



void CExplosive::SetDirection(Vector3 theNewDirection)
{
	this->theDirection = theNewDirection;
}

Vector3 CExplosive::GetDirection(void)
{
	return this->theDirection;
}
void CExplosive::SetStatus(const bool m_bStatus)
{
	if (m_bStatus == false)
		m_fLifetime = -1;
	this->m_bStatus = m_bStatus;
}

bool CExplosive::GetStatus(void) const
{
	return m_bStatus;
}

CExplosive * Create::Explosive(const std::string & _meshName, const Vector3 & _position, const Vector3 & _direction, const float m_fLifetime, const float m_fSpeed, const float mass, CPlayerInfo * _source)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	CExplosive* result = new CExplosive(modelMesh);
	result->Set(_position, _direction, m_fLifetime, m_fSpeed, mass);
	result->SetStatus(true);
	result->SetCollider(true);
	result->SetSource(_source);
	result->SetEntityType(ECEntityTypes::OTHERS);
	
	//result->SetRotateAngle(90.f);
	//result->SetRotateAxis(Vector3(1.f, 0.f, 0.f));
	EntityManager::GetInstance()->AddEntity(result);

	return result;
}



void CExplosive::SetLifetime(const float m_fLifetime)
{
	this->m_fLifetime = m_fLifetime;
}

float CExplosive::GetLifetime(void) const
{
	return this->m_fLifetime;
}

void CExplosive::SetSpeed(const float m_fSpeed)
{
	this->m_fSpeed = m_fSpeed;
}

float CExplosive::GetSpeed(void) const
{
	return m_fSpeed;
}

void CExplosive::SetSource(CPlayerInfo * _source)
{
	this->theSource = _source;
}

CPlayerInfo * CExplosive::GetSource(void) const
{
	return theSource;
}
