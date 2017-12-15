#include "Explosive.h"

#include "../SpatialPartition/SpatialPartitionManager.h"
#include "Manager\CollisionManager.h"
#include "../QuadTree/QuadTreeManager.h"
#include "Collider\Collider.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../EntityTypes/EntityTypes.h"
#include "MeshBuilder.h"
#include "../AudioPlayer/AudioPlayer.h"
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
	//this->collider = new CCollider(this);
	//this->collider->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
	//CollisionManager::GetInstance()->AddCollider(this->collider, this->GetPartitionPtr());

	//for legit collision
	//QuadTreeManager::GetInstance()->InsertEntity(this);
	//this->SetAABB(Vector3(-1, -1, -1), Vector3(1, 1, 1));

	this->m_bStatus = true;
	this->isDone = false;

}

void CExplosive::Update(double dt)
{
	m_fLifetime -= dt;
	if (m_fLifetime <= 0.0)
	{
		//this->SetIsDone(true);

		//static std::vector<std::pair<Vector3, Vector3>> lines;

		if (boom == false)
		{
			CAudioPlayer::GetInstance()->GetISoundEngine()->play2D("Audio/SFX/EXPLODE.ogg", false);

			this->collider->SetAABBPosition(this->position);
			this->collider->SetAABB(this->position + Vector3(100, 100, 100), this->position - Vector3(100, 100, 100));
			std::list<EntityBase*> elist(QuadTreeManager::GetInstance()->GetNearbyEntities(this, QuadTreeManager::GetInstance()->GetRoot(), true));
			//std::cout << "eList Size : " << elist.size() << std::endl;
			//for (auto e : elist)
			//{
			//	if (e == this)
			//		continue;
			//	lines.push_back(std::make_pair(this->position, e->GetPosition()));
			//	//e->SetIsDone(true);
			//}
			boom = true;

			//std::cout << "lines Size : " << lines.size() << std::endl;
		}

		//CollisionManager::GetInstance()->posColliderChecks.insert(CollisionManager::GetInstance()->posColliderChecks.end(), lines.begin(), lines.end());

		//for (auto lp : lines)
		//	std::cout << "lines " << lp.first << " , " << lp.second << std::endl;


		//if (m_fLifetime < 0.0)
			this->SetIsDone(true);
	}

	if (m_bStatus == false)
		return;

	// Update Position
	Vector3 velocity = theDirection  * m_fSpeed;
	Vector3 acceleration = Vector3(0, -10, 0) * mass;
	velocity = velocity + acceleration * dt;
	theDirection = velocity.Normalized();
	position += velocity * dt;
	//position.Set(position.x + (float)(theDirection.x * dt * m_fSpeed),
	//	position.y + (float)(theDirection.y * dt * m_fSpeed),
	//	position.z + (float)(theDirection.z * dt * m_fSpeed));
	//theDirection.y -= mass * dt;
	//position.y += -9.0 * mass * dt;

	this->ClearPartition();
	if (CSpatialPartitionManager::GetInstance()->UpdateGridInfo(position))
		this->SetPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(position)->GetIndex());

	this->collider->SetAABBPosition(this->position);
	this->collider->SetAABB(this->position + scale * 0.5f, this->position + -scale * 0.5f);
	//this->collider->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
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
	//if (m_bStatus == false)
	//	m_fLifetime = -1;
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
	result->SetName(_meshName);
	result->SetPosition(_position);
	result->SetScale(Vector3(5.f, 5.f, 5.f));
	result->SetCollider(false);
	/*Min AABB followed by Max AABB.*/
	Vector3 _maxAABB(0.5f, 0.5f, 0.5f);
	//result->SetAABB(Vector3(-_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f),
	//Vector3(_maxAABB.x * 0.5f, _maxAABB.x * 0.5f, _maxAABB.x * 0.5f));
	QuadTreeManager::GetInstance()->InsertEntity(result);
	result->SetIsParent(true);


	/*Create root for Scene Graph.*/
	CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
	rootNode->GetEntity()->SetPosition(_position);
	/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
	The entire Scene Graph will be traversed from the root.*/
	result->SetSceneGraph(Create::SceneGraph(rootNode));
	Create::SceneNode(rootNode, rootNode, result);
	/*Init the LoD.*/
	result->InitLoD(_meshName, _meshName, _meshName);
	/*Set AABB.*/
	result->SetEntityType(ECEntityTypes::OBJECT);
	result->collider = new CCollider(result);
	//result->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	result->collider->SetMinAABB(Vector3(-result->GetScale() * 0.5f) + result->GetPosition());
	result->collider->SetMaxAABB(Vector3(result->GetScale() * 0.5f) + result->GetPosition());
	result->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(result->GetPosition(), result->GetScale()));
	CollisionManager::GetInstance()->AddCollider(result->collider, result->GetPartitionPtr());


	result->Set(_position, _direction, m_fLifetime, m_fSpeed, mass);

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
