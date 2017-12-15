#include "Monk.h"
#include "MeshBuilder.h"
#include "Manager\CollisionManager.h"
#include "../../SpatialPartition/SpatialPartitionManager.h"
#include "../../EntityTypes/EntityTypes.h"
#include "../EnemyManager.h"
#include "../../QuadTree/QuadTreeManager.h"
#include "../../AudioPlayer/AudioPlayer.h"

CMonk::CMonk() : head(nullptr)
, body(nullptr)
, leftArm(nullptr)
, rightArm(nullptr)
, leftLeg(nullptr)
, rightLeg(nullptr)
, player(nullptr)
{
}

CMonk::~CMonk()
{
}

void CMonk::SetModelMeshes(Mesh * _head, Mesh * _body, Mesh * _leftArm, Mesh * _rightArm, Mesh * _leftLeg, Mesh * _rightLeg)
{
	headModelMesh = _head;
	bodyModelMesh = _body;
	leftArmModelMesh = _leftArm;
	rightArmModelMesh = _rightArm;
	leftLegModelMesh = _leftLeg;
	rightLegModelMesh = _rightLeg;
}

GenericEntity * CMonk::GetEntityPart(std::string _part)
{
	if (_part == "MONK_HEAD") {
		return head;
	}
	else if (_part == "MONK_BODY") {
		return body;
	}
	else if (_part == "MONK_LEFT_ARM") {
		return leftArm;
	}
	else if (_part == "MONK_RIGHT_ARM") {
		return rightArm;
	}
	else if (_part == "MONK_LEFT_LEG") {
		return leftLeg;
	}
	else if (_part == "MONK_RIGHT_LEG") {
		return rightLeg;
	}
	return nullptr;
}

void CMonk::SetEntityPart(std::string _part, GenericEntity * _entity)
{
	if (_part == "MONK_HEAD") {
		head = _entity;
	}
	else if (_part == "MONK_BODY") {
		body = _entity;
	}
	else if (_part == "MONK_LEFT_ARM") {
		leftArm = _entity;
	}
	else if (_part == "MONK_RIGHT_ARM") {
		rightArm = _entity;
	}
	else if (_part == "MONK_LEFT_LEG") {
		leftLeg = _entity;
	}
	else if (_part == "MONK_RIGHT_LEG") {
		rightLeg = _entity;
	}
}

void CMonk::UpdatePart(double dt, std::string _part)
{
	float rotateAngle = 0.f;
	/*Update the main big invisible part*/
	//this->collider->SetAABBPosition(this->position);
	bool deleteTheRest = false;
	for (std::vector<GenericEntity*>::iterator it = partList.begin(); it != partList.end(); )
	{
		if ((*it)->isDone || deleteTheRest == true)
		{
			GenericEntity* del = *it;
			del->scale -= Vector3(5, 5, 5) * dt;
			if (del->scale.LengthSquared() < 0.1f)
				del->scale.Set(0.01f, 0.01f, 0.01f);

			if (del->scale.LengthSquared() < 0.1f)
			{

				head->rootNode->DeleteChild(del);
				it = partList.erase(it);
				
				if (del->name == "MONK_HEAD")
				{
					deleteTheRest = true;
					head = nullptr;
				}
				else if (del->name == "MONK_LEFT_LEG")
				{
					leftLeg = nullptr;
				}
				else if (del->name == "MONK_RIGHT_LEG")
				{
					rightLeg = nullptr;
				}
				delete del;
				del = nullptr;
			}
			else
				++it;

		}
		else
			++it;
	}
	if (partList.empty() || head == nullptr)
		return;

	/*Update the direction of each part.*/
	for each( GenericEntity* part in partList) {
		Vector3 tempPosition(0.f, 0.f, 0.f);
		try {
			rotateAngle = Math::RadianToDegree(atan2((player->GetPos().x - part->GetPosition().x), (player->GetPos().z - part->GetPosition().z)));
		}
		catch (std::string e) {

		}
		if (part->GetName() != "MONK_HEAD")
			part->SetRotateAngle(rotateAngle);
		part->SetRotateAxis(1.f, 1.f, 0.f);

		if (part->GetName() == "MONK_HEAD") {
			tempPosition = head->GetPosition();
			/*Set AABB Position to be at the center of the object.*/
			part->collider->SetAABBPosition(tempPosition.x, tempPosition.y + 13.f, tempPosition.z);
			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 2.5f, 
				part->collider->GetAABBPosition().y + 2.5f, 
				part->collider->GetAABBPosition().z + 2.5f),

				Vector3(part->collider->GetAABBPosition().x - 2.5f,
					part->collider->GetAABBPosition().y - 2.5f,
					part->collider->GetAABBPosition().z - 2.5f));
		}
		else if (part->GetName() == "MONK_BODY") {
			tempPosition = body->GetPosition();
			/*Set AABB Position to be at the center of the object.*/
			part->collider->SetAABBPosition(tempPosition.x, tempPosition.y + 5.f, tempPosition.z);
			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 2.5f,
				part->collider->GetAABBPosition().y + 4.f,
				part->collider->GetAABBPosition().z + 2.5f),

				Vector3(part->collider->GetAABBPosition().x - 2.5f,
					part->collider->GetAABBPosition().y,
					part->collider->GetAABBPosition().z - 2.5f));
		}
		else if (part->GetName() == "MONK_LEFT_ARM") {
			tempPosition = leftArm->GetPosition();
			part->collider->SetAABBPosition((5.f * cos(Math::DegreeToRadian(-leftArm->GetRotateAngle())) + tempPosition.x),
				tempPosition.y + 7.f,
				(5.f * sin(Math::DegreeToRadian(-leftArm->GetRotateAngle()))) + tempPosition.z);
			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 1.f,
				part->collider->GetAABBPosition().y + 1.f,
				part->collider->GetAABBPosition().z + 1.f),

				Vector3(part->collider->GetAABBPosition().x - 1.f,
					part->collider->GetAABBPosition().y - 1.f,
					part->collider->GetAABBPosition().z - 1.f));
		}
		else if (part->GetName() == "MONK_RIGHT_ARM") {
			tempPosition = rightArm->GetPosition();
			part->collider->SetAABBPosition((-5.f * cos(Math::DegreeToRadian(-rightArm->GetRotateAngle())) + tempPosition.x),
				tempPosition.y + 7.f,
				(-5.f * sin(Math::DegreeToRadian(-rightArm->GetRotateAngle()))) + tempPosition.z);
			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 1.f,
				part->collider->GetAABBPosition().y + 1.f,
				part->collider->GetAABBPosition().z + 1.f),

				Vector3(part->collider->GetAABBPosition().x - 1.f,
					part->collider->GetAABBPosition().y - 1.f,
					part->collider->GetAABBPosition().z - 1.f));
		}
		else if (part->GetName() == "MONK_LEFT_LEG") {
			tempPosition = leftLeg->GetPosition();
			part->collider->SetAABBPosition((1.2 * cos(Math::DegreeToRadian(-leftArm->GetRotateAngle())) + tempPosition.x),
				tempPosition.y + 2.5f,
				(1.2 * sin(Math::DegreeToRadian(-leftArm->GetRotateAngle()))) + tempPosition.z);

			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 1.f,
				part->collider->GetAABBPosition().y + 2.f,
				part->collider->GetAABBPosition().z + 1.f),

				Vector3(part->collider->GetAABBPosition().x - 1.f,
					part->collider->GetAABBPosition().y - 2.5f,
					part->collider->GetAABBPosition().z - 1.f));
		}
		else if (part->GetName() == "MONK_RIGHT_LEG") {
			tempPosition = rightLeg->GetPosition();
			part->collider->SetAABBPosition((-1.2f * cos(Math::DegreeToRadian(-rightLeg->GetRotateAngle())) + tempPosition.x),
				tempPosition.y + 2.5f,
				(-1.2f * sin(Math::DegreeToRadian(-rightLeg->GetRotateAngle()))) + tempPosition.z);
			/*Set the min and max based on AABB Position.*/
			part->collider->SetAABB(Vector3(part->collider->GetAABBPosition().x + 1.f,
				part->collider->GetAABBPosition().y + 2.f,
				part->collider->GetAABBPosition().z + 1.f),

				Vector3(part->collider->GetAABBPosition().x - 1.f,
					part->collider->GetAABBPosition().y - 2.5f,
					part->collider->GetAABBPosition().z - 1.f));
		}
	}

	head->SetRotateAngle(head->GetRotateAngle() + static_cast<float>(dt) * 50.f);
	head->SetRotateAxis(0.f, 1.f, 0.f);
	head->AddTimer(dt);
	if (head->GetTimer() > 0.5f) {
		head->ResetTimer();
		head->ToggleTransformChange();
	}
	if (!head->GetTransformChange())
		head->SetDirection(head->translateAnimation["MOVE_UP"] * 10.f);
	else
		head->SetDirection(head->translateAnimation["MOVE_DOWN"] * 10.f);

	/*Make the entire enemy move in square.*/
	/*Does not matter which body parts is being used to get Root because it will be the same.*/
	EntityBase* core = head->GetRootNode()->GetEntity();
	static int directionIndex = 0;
	static float timer = 0.f;
	static bool translateSwitch = false;
	timer += static_cast<float>(dt);

	Vector3 moveToPlayer = (Vector3(player->GetPos().x, core->GetPosition().y, player->GetPos().z) - core->GetPosition()) * 2.f;
	try {
		moveToPlayer = moveToPlayer.Normalized();
	}
	catch (std::string e) {
		/*Do nothing when object and player is at same position.*/
	}

	if ((leftLeg == rightLeg) && rightLeg == nullptr)
	{
		core->SetDirection(0, 0, 0);
		return;
	}
	if ((Vector3(player->GetPos().x, core->GetPosition().y, player->GetPos().z) - core->GetPosition()).LengthSquared() > 100.f)
		core->SetDirection(moveToPlayer * 20.f);
	else
		core->SetDirection(0.f, 0.f, 0.f);

	/*Rotate the legs to animate as walking.*/

	if (leftLeg != nullptr)
	{
		if (leftLeg->GetTransformOffset() >= 10.f)
			leftLeg->SetTransformChange(true);
		else if (leftLeg->GetTransformOffset() <= leftLeg->GetTimer())
			leftLeg->SetTransformChange(false);

		if (!leftLeg->GetTransformChange()) {
			leftLeg->AddTransformOffset(dt * 50.f);
			leftLeg->SetTimer(-10.f);
		}
		else
			leftLeg->AddTransformOffset(-dt * 50.f);
		leftLeg->SetOffset(leftLeg->GetTransformOffset(), 0.f, 0.f);
	}

	if (rightLeg != nullptr)
	{
		if (rightLeg->GetTransformOffset() <= -10.f)
			rightLeg->SetTransformChange(true);
		else if (rightLeg->GetTransformOffset() >= rightLeg->GetTimer())
			rightLeg->SetTransformChange(false);

		if (!rightLeg->GetTransformChange()) {
			rightLeg->AddTransformOffset(-dt * 50.f);
			rightLeg->SetTimer(10.f);
		}
		else
			rightLeg->AddTransformOffset(dt * 50.f);
		rightLeg->SetOffset(rightLeg->GetTransformOffset(), 0.f, 0.f);
	}
	if (_part == "MONK_HEAD") {

	}
	else if (_part == "MONK_BODY") {

	}
	else if (_part == "MONK_LEFT_ARM") {

	}
	else if (_part == "MONK_RIGHT_ARM") {

	}
	else if (_part == "MONK_LEFT_LEG") {

	}
	else if (_part == "MONK_RIGHT_LEG") {

	}
}

void CMonk::CollisionResponse(EntityBase * other)
{
	std::cout << "BOOOM " << std::endl;

	if (name == "MONK_BODY" ||
		name == "MONK_LEFT_ARM" ||
		name == "MONK_RIGHT_ARM" ||
		name == "MONK_LEFT_LEG" ||
		name == "MONK_RIGHT_LEG")
	{
		CAudioPlayer::GetInstance()->GetISoundEngine()->play2D("Audio/SFX/DAMAGED.ogg", false);
	}
	else if (name == "MONK_HEAD")
		CAudioPlayer::GetInstance()->GetISoundEngine()->play2D("Audio/SFX/DEATH.ogg", false);

	CollisionManager::GetInstance()->CheckCollision(other->collider, head->collider, 1.0/6.0);
	CollisionManager::GetInstance()->CheckCollision(other->collider, body->collider, 1.0 / 6.0);
	CollisionManager::GetInstance()->CheckCollision(other->collider, leftArm->collider, 1.0 / 6.0);
	CollisionManager::GetInstance()->CheckCollision(other->collider, rightArm->collider, 1.0 / 6.0);
	CollisionManager::GetInstance()->CheckCollision(other->collider, leftLeg->collider, 1.0 / 6.0);
	CollisionManager::GetInstance()->CheckCollision(other->collider, rightLeg->collider, 1.0 / 6.0);
}

CMonk * Create::Monk(const Vector3 & _position, const Vector3 & _scale, CPlayerInfo* _player)
{
	CMonk* monk = new CMonk();
	monk->SetModelMeshes(MeshBuilder::GetInstance()->GetMesh("MONK_HEAD"),
		MeshBuilder::GetInstance()->GetMesh("MONK_BODY"),
		MeshBuilder::GetInstance()->GetMesh("MONK_LEFT_ARM"),
		MeshBuilder::GetInstance()->GetMesh("MONK_RIGHT_ARM"),
		MeshBuilder::GetInstance()->GetMesh("MONK_LEFT_LEG"),
		MeshBuilder::GetInstance()->GetMesh("MONK_RIGHT_LEG"));

	GenericEntity* tempHead = Create::Asset("MONK_HEAD", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f), true);
	monk->SetEntityPart("MONK_HEAD", tempHead);

	GenericEntity* tempBody = Create::Asset("MONK_BODY", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f));
	monk->SetEntityPart("MONK_BODY", tempBody);

	GenericEntity* tempLeftArm = Create::Asset("MONK_LEFT_ARM", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f));
	monk->SetEntityPart("MONK_LEFT_ARM", tempLeftArm);

	GenericEntity* tempRightArm = Create::Asset("MONK_RIGHT_ARM", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f));
	monk->SetEntityPart("MONK_RIGHT_ARM", tempRightArm);

	GenericEntity* tempLeftLeg = Create::Asset("MONK_LEFT_LEG", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f));
	monk->SetEntityPart("MONK_LEFT_LEG", tempLeftLeg);

	GenericEntity* tempRightLeg = Create::Asset("MONK_RIGHT_LEG", _position, Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f));
	monk->SetEntityPart("MONK_RIGHT_LEG", tempRightLeg);


	/*Create root for Scene Graph.*/
	CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
	rootNode->GetEntity()->SetPosition(_position);

	/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
	The entire Scene Graph will be traversed from the root.*/
	tempHead->SetSceneGraph(Create::SceneGraph(rootNode));
	Create::SceneNode(rootNode, rootNode, tempHead);
	/*Init the LoD.*/
	tempHead->InitLoD("MONK_HEAD", "MONK_HEAD_MID", "MONK_HEAD_LOW");
	/*Set AABB.*/
	tempHead->SetEntityType(ECEntityTypes::OBJECT);
	tempHead->collider = new CCollider(tempHead);
	//tempHead->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempHead->collider->SetMinAABB(Vector3(-tempHead->GetScale() * 0.5f) + tempHead->GetPosition());
	tempHead->collider->SetMaxAABB(Vector3(tempHead->GetScale() * 0.5f) + tempHead->GetPosition());
	tempHead->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempHead->GetPosition(), tempHead->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempHead->collider, tempHead->GetPartitionPtr());

	/*Create node and bind it to parent and root.*/
	Create::SceneNode(rootNode, rootNode, tempBody);
	/*Body.*/
	tempBody->InitLoD("MONK_BODY", "MONK_BODY_MID", "MONK_BODY_LOW");
	/*Set AABB.*/
	tempBody->SetEntityType(ECEntityTypes::OBJECT);
	tempBody->collider = new CCollider(tempBody);
	//tempBody->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempBody->collider->SetMinAABB(Vector3(-tempBody->GetScale() * 0.5f) + tempBody->GetPosition());
	tempBody->collider->SetMaxAABB(Vector3(tempBody->GetScale() * 0.5f) + tempBody->GetPosition());
	tempBody->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempBody->GetPosition(), tempBody->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempBody->collider, tempBody->GetPartitionPtr());

	/*Create node and bind it to parent and root.*/
	Create::SceneNode(rootNode, rootNode, tempLeftArm);
	/*Left Arm.*/
	tempLeftArm->InitLoD("MONK_LEFT_ARM", "MONK_LEFT_ARM_MID", "MONK_LEFT_ARM_LOW");
	/*Set AABB.*/
	tempLeftArm->SetEntityType(ECEntityTypes::OBJECT);
	tempLeftArm->collider = new CCollider(tempLeftArm);
	//tempLeftArm->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempLeftArm->collider->SetMinAABB(Vector3(-tempLeftArm->GetScale() * 0.5f) + tempLeftArm->GetPosition());
	tempLeftArm->collider->SetMaxAABB(Vector3(tempLeftArm->GetScale() * 0.5f) + tempLeftArm->GetPosition());
	tempLeftArm->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempLeftArm->GetPosition(), tempLeftArm->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempLeftArm->collider, tempLeftArm->GetPartitionPtr());

	/*Create node and bind it to parent and root.*/
	Create::SceneNode(rootNode, rootNode, tempRightArm);
	/*Right Arm.*/
	tempRightArm->InitLoD("MONK_RIGHT_ARM", "MONK_RIGHT_ARM_MID", "MONK_RIGHT_ARM_LOW");
	/*Set AABB.*/
	tempRightArm->SetEntityType(ECEntityTypes::OBJECT);
	tempRightArm->collider = new CCollider(tempRightArm);
	//tempRightArm->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempRightArm->collider->SetMinAABB(Vector3(-tempRightArm->GetScale() * 0.5f) + tempRightArm->GetPosition());
	tempRightArm->collider->SetMaxAABB(Vector3(tempRightArm->GetScale() * 0.5f) + tempRightArm->GetPosition());
	tempRightArm->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempRightArm->GetPosition(), tempRightArm->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempRightArm->collider, tempRightArm->GetPartitionPtr());

	/*Create node and bind it to parent and root.*/
	Create::SceneNode(rootNode, rootNode, tempLeftLeg);
	/*Left Leg.*/
	tempLeftLeg->InitLoD("MONK_LEFT_LEG", "MONK_LEFT_LEG", "MONK_LEFT_LEG");
	/*Set AABB.*/
	tempLeftLeg->SetEntityType(ECEntityTypes::OBJECT);
	tempLeftLeg->collider = new CCollider(tempLeftLeg);
	//tempLeftLeg->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempLeftLeg->collider->SetMinAABB(Vector3(-tempLeftLeg->GetScale() * 0.5f) + tempLeftLeg->GetPosition());
	tempLeftLeg->collider->SetMaxAABB(Vector3(tempLeftLeg->GetScale() * 0.5f) + tempLeftLeg->GetPosition());
	tempLeftLeg->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempLeftLeg->GetPosition(), tempLeftLeg->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempLeftLeg->collider, tempLeftLeg->GetPartitionPtr());

	/*Create node and bind it to parent and root.*/
	Create::SceneNode(rootNode, rootNode, tempRightLeg);
	/*Right Leg.*/
	tempRightLeg->InitLoD("MONK_RIGHT_LEG", "MONK_RIGHT_LEG", "MONK_RIGHT_LEG");
	/*Set AABB.*/
	tempRightLeg->SetEntityType(ECEntityTypes::OBJECT);
	tempRightLeg->collider = new CCollider(tempRightLeg);
	//tempRightLeg->SetAABB(Vector3(-2.5f, -2.5f, -2.5f), Vector3(2.5f, 2.5f, 2.5f));
	tempRightLeg->collider->SetMinAABB(Vector3(-tempRightLeg->GetScale() * 0.5f) + tempRightLeg->GetPosition());
	tempRightLeg->collider->SetMaxAABB(Vector3(tempRightLeg->GetScale() * 0.5f) + tempRightLeg->GetPosition());
	tempRightLeg->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tempRightLeg->GetPosition(), tempRightLeg->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tempRightLeg->collider, tempRightLeg->GetPartitionPtr());

	monk->AddPartToList(tempHead);
	monk->AddPartToList(tempBody);
	monk->AddPartToList(tempLeftArm);
	monk->AddPartToList(tempRightArm);
	monk->AddPartToList(tempLeftLeg);
	monk->AddPartToList(tempRightLeg);

	tempHead->SetTransformOffset(0.25f);
	/*Define Animation Pattern.*/
	for each(GenericEntity* part in monk->GetPartList()) {
		part->translateAnimation["MOVE_UP"] = Vector3(0.f, 1.f, 0.f);
		part->translateAnimation["MOVE_DOWN"] = Vector3(0.f, -1.f, 0.f);
		part->translateAnimation["MOVE_LEFT"] = Vector3(-1.f, 0.f, 0.f);
		part->translateAnimation["MOVE_RIGHT"] = Vector3(1.f, 0.f, 0.f);

		part->isStatic = false;
		part->isDestroyable = true;
	}

	/*Add into Enemy Manager for State Changes Update.*/
	CEnemyManager::GetInstance()->AddEnemy(monk);
	//monk->SetAABB(-_scale *0.5f, _scale * 0.5f);
	//monk->SetAABB(-Vector3(10,0,10) *0.5f, Vector3(10, 20, 10) * 0.5f);
	//monk->collider = new CCollider(monk);
	//monk->collider->SetAABB(Vector3(10, 0, 10) *0.5f, -Vector3(10, 20, 10) * 0.5f);
	//QuadTreeManager::GetInstance()->InsertEntity(monk);

	/*Set AABB.*/
	rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
	rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
	rootNode->GetEntity()->collider->SetMinAABB(Vector3(-5.f, 0.f, -5.f));
	rootNode->GetEntity()->collider->SetMaxAABB(Vector3(5.f, 20.f, 5.f));
	rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
	rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
	rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
	CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
	//QuadTreeManager::GetInstance()->InsertEntity(rootNode->GetEntity());
	tempHead->isStatic = true;
	rootNode->GetEntity()->rootNode = rootNode;

	/*Assign player to pointer for tracking position.*/
	monk->SetPlayer(_player);
	return monk;
}
