#include "QuadTreeManager.h"

#include "EntityBase.h"
#include "RenderHelper.h"
#include "Manager\CollisionManager.h"
#include "../GenericEntity.h"
#include "GraphicsManager.h"
#include "CameraManager.h"
#include "../Enemy/EnemyManager.h"
#include "CameraManager.h"
#include "../FPSCamera.h"
#include "MeshBuilder.h"
#include "../GroundEntity.h"
#include "../Projectile/Explosive.h"
#include "../../Common/Source/MeshBuilder.h"
QuadTreeManager* QuadTreeManager::instance = nullptr;

QuadTreeManager::QuadTreeManager() : root(nullptr), entityList(entityList), minSplitSize(2U)
{
}

QuadTreeManager::~QuadTreeManager()
{
}

void QuadTreeManager::Update(double dt)
{
	//delete tree
	if (root)
	{
		//delete
		root->DeleteChildren();
		delete root;
		root = nullptr;
	}

	//Delete them
	
	for (auto & e : preUpdateRemovalList)
	{
		//this->RemoveEntity(e);
		CEnemyManager::GetInstance()->RemoveEnemy(e);
		//if (e)
		//{
		//	delete e;
		//	e = nullptr;
		//}
	}
	preUpdateRemovalList.clear();

	//recreate tree
	if (entityList.size())
	{
		std::list<EntityBase*> eList = this->GetEntityList();
		root = new QuadTree(Vector3(-200, 0, -200), Vector3(-200, 0, -200), Vector3(200, 0, 200), 0, 4, eList);
		root->groundMesh = MeshBuilder::GetInstance()->GetMesh("COMGROUND");
		//check split
		this->CheckTreeNode(root);

		//root->Update(dt);
	}

	//to determine which node to render or no
	this->UpdateLeafNode(root, dt);

	/*Update all the entities through Scene Graph.*/
	for (auto e : entityList)
	{
		if (dynamic_cast<GenericEntity*>(e)) {
			GenericEntity* castedEntity = dynamic_cast<GenericEntity*>(e);
			if (castedEntity->GetSceneGraph() != nullptr) {
				castedEntity->UpdateAllNodes(castedEntity->GetRootNode(), dt);
			}
		}
	}

	CEnemyManager::GetInstance()->Update(dt);
	renderCout = false;


	for (std::vector<AOEQuad>::iterator it = renderList.begin(); it != renderList.end();)
	{
		(*it).dt -= dt;
		if ((*it).dt <= 0.0)
		{
			it = renderList.erase(it);
		}
		else
			++it;
	}

	for (std::vector<AOEQuad>::iterator it = hitScanList.begin(); it != hitScanList.end(); )
	{
		(*it).dt -= dt;
		if ((*it).dt <= 0.0)
			it = hitScanList.erase(it);
		else
			++it;
	}
}

void QuadTreeManager::PostUpdate(double dt)
{
	for (int i = 0; i < entityList.size();)
	{
		if (entityList[i]->IsDone()) {
			if (entityList[i]->GetChildren().size() > 0) {
				std::vector<CSceneNode*>childrenList = entityList[i]->GetChildren();
				for (auto child : childrenList) {
					this->RemoveEntity(child->GetEntity());
					EntityBase* eChild = child->GetEntity();
					delete eChild;
					eChild = nullptr;
				}
			}
			EntityBase* del = entityList[i];
			preUpdateRemovalList.push_back(del);
			this->RemoveEntity(entityList[i]);
			//CEnemyManager::GetInstance()->RemoveEnemy(del);
		}
		else
			++i;
	}
}

void QuadTreeManager::CheckTreeNode(QuadTree * node)
{
	if (!node)
		return;

	if (node->entityList.size() >= minSplitSize)
	{
		if (renderCout)
			std::cout << "Pos : " << node->position << "  Min : " << node->minBoundary <<
			"  Max : " << node->maxBoundary << std::endl;
		node->Split();

		this->CheckTreeNode(node->topLeft);
		this->CheckTreeNode(node->topRight);
		this->CheckTreeNode(node->bottomLeft);
		this->CheckTreeNode(node->bottomRight);
	}
}

std::list<EntityBase*> QuadTreeManager::GetEntityList()
{
	std::list<EntityBase*> eList;
	for (int i = 0; i < this->entityList.size(); ++i)
	{
		//can edit who to give or who not to give
		eList.push_back(this->entityList[i]);
	}
	return eList;
}

std::vector<Vector3> QuadTreeManager::GetGridVertices(QuadTree * node)
{
	std::vector<Vector3> result;

	if (node == nullptr)
		return result;

	//push ur vertices back
	Vector3 min = node->GetMinBoundary();
	Vector3 max = node->GetMaxBoundary();
	result.push_back(Vector3(min.x,0, min.z));
	result.push_back(Vector3(min.x,0, max.z));
	result.push_back(Vector3(max.x,0, max.z));
	result.push_back(Vector3(max.x,0, min.z));

	std::vector<QuadTree*> childArray (node->GetAllChildren());
	if (childArray.empty())
		return result;
	for (int i = 0; i < childArray.size(); ++i)
	{
		std::vector<Vector3> childVertices(GetGridVertices(childArray[i]));
		result.insert(result.end(), childVertices.begin(), childVertices.end());
	}
	
	return result;
}

void QuadTreeManager::RenderGrid()
{
	std::vector<Vector3> allVertices(this->GetGridVertices(root));

	//Can be disabled
	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	ms.PushMatrix();
	ms.Translate(0, -9.5f, 0);
	RenderHelper::DrawLine(allVertices, Color(1, 0, 0), 4);
	ms.PopMatrix();

	ms.PushMatrix();
	ms.Translate(0 + CameraManager::GetInstance()->GetPlayerCam()->GetCameraPos().x,
		800.f + CameraManager::GetInstance()->GetPlayerCam()->GetCameraPos().y,
		0 + CameraManager::GetInstance()->GetPlayerCam()->GetCameraPos().z);
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("SKYPLANE"));
	ms.PopMatrix();

	//for (std::deque<EntityBase*>::iterator it = entityList.begin(); it != entityList.end(); ++it) {
	//	if (dynamic_cast<GenericEntity*>((*it))) {
	//		GenericEntity* castedEntity = dynamic_cast<GenericEntity*>((*it));
	//		if (castedEntity->GetSceneGraph() != nullptr) {
	//			castedEntity->RenderChildren();
	//		}
	//	}
	//}
//	RenderGrid(root);
	this->RenderObj();

	//std::cout << "renderQuadList : " <<  renderList.size() << std::endl;
	for (std::vector<AOEQuad>::iterator it = renderList.begin(); it != renderList.end(); ++it)
	{
		ms.PushMatrix();
		ms.Translate(0, -9.5, 0);
		ms.Translate((*it).pos);
		ms.Translate((*it).scale.x * 0.5, 0, (*it).scale.z * 0.5f);
		ms.Scale((*it).scale);
		ms.Rotate(-90, 1,0,0);


		//std::cout << "pos " << (*it).pos << " scale " << (*it).scale << std::endl;
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("OQUAD"));
		//RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("MONK_HEAD"));
		ms.PopMatrix();

		//std::cout << "renderQuadList : " << renderList.size() << std::endl;
	}

	for (std::vector<AOEQuad>::iterator it = hitScanList.begin(); it != hitScanList.end(); ++it)
	{
		ms.PushMatrix();
		ms.Translate((*it).pos);
		ms.Scale((*it).scale);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("SPHERE"));
		ms.PopMatrix();
	}
}

/*Dieded.*/
void QuadTreeManager::RenderObj()
{
	RenderObj(root);

	//if (root == nullptr)
	//{
		MS& ms = GraphicsManager::GetInstance()->GetModelStack();
		ms.PushMatrix();
		Vector3 scale(400, 0, 400);
		//ms.Translate(this->maxBoundary - this->minBoundary * 0.5f);
		ms.Translate(0, -10, 0);
		ms.Scale(scale);
		ms.Rotate(-90, 1, 0, 0);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("COMGROUND"));
		ms.PopMatrix();
	//}
}

void QuadTreeManager::Clear()
{
	ClearEntities();
	renderList.clear();
	hitScanList.clear();
	ground = nullptr;
	toggle = true;
}


void QuadTreeManager::RenderObj(QuadTree * node)
{
	if (!node)
		return;

	std::vector<QuadTree*> children(node->GetAllChildren());
	if (!children.empty())
	{
		for (auto child : children)
		{
			RenderObj(child);
		}
		return;
	}

	// At leaf Node

	if (!node->RenderGrid)
		return;

	//render ground
	//node->Render();

	//this is not using scenenode. remove later
	for (auto e : node->entityList)
	{
		//can check if obj is inside ah
		//e->Render();
		if (dynamic_cast<GenericEntity*>(e)) {
			GenericEntity* castedEntity = dynamic_cast<GenericEntity*>(e);
			if (castedEntity->GetSceneGraph() != nullptr) {
				castedEntity->RenderAllNodes(castedEntity->GetRootNode());
			}
		}
	}

	//RenderObj(node->topLeft);
	//RenderObj(node->topRight);
	//RenderObj(node->bottomLeft);
	//RenderObj(node->bottomRight);
}

void QuadTreeManager::UpdateLeafNode(QuadTree * node, double dt)
{
	if (!node)
		return;
	if (node->topLeft)
	{
		std::vector<QuadTree*> children(node->GetAllChildren());
		for (auto child : children)
		{
			UpdateLeafNode(child, dt);
		}
		return;
	}

	node->Update(dt);
}

int QuadTreeManager::PutEntitiesInGrid(QuadTree * node, std::list<EntityBase*>& entityList)
{
	//here we extract the entites that fall inside the node and put them inside
	Vector3 pos = node->GetPosition();
	Vector3 min = node->GetMinBoundary();
	Vector3 max = node->GetMaxBoundary();

	int pushed = 0;
	for (std::list<EntityBase*>::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		EntityBase* e = (*it);

		if (e->GetPosition().x >= min.x && e->GetPosition().y >= min.y && e->GetPosition().z >= min.z
			&& e->GetPosition().x < max.x && e->GetPosition().y < max.y && e->GetPosition().z < max.z) {
			++pushed;
			node->entityList.push_back(e);
		}
	}

	return pushed;
}

void QuadTreeManager::CheckCollision(std::vector<std::pair<Vector3, Vector3>>& posOfChecks, double dt)
{
	if (!root)
		return;

	std::vector<std::pair<Vector3, Vector3>> ret(CheckCollision(root, dt));
	posOfChecks.assign(ret.begin(), ret.end());
}

#include <Windows.h>
#include "SceneManager.h"
#include "EntityBase.h"
#include "../GenericEntity.h"
std::vector<std::pair<Vector3, Vector3>> QuadTreeManager::CheckCollision(QuadTree * node, double dt)
{
	std::vector<std::pair<Vector3, Vector3>> posOfChecks;

	if (!node)
		return posOfChecks;

	//check leaf node only
	if (node->GetAllChildren().size()) {
		
		std::vector<std::pair<Vector3, Vector3>> temp (CheckCollision(node->topLeft, dt));
		posOfChecks.insert(posOfChecks.end(), temp.begin(), temp.end());
		std::vector<std::pair<Vector3, Vector3>> temp2(CheckCollision(node->topRight, dt));
		posOfChecks.insert(posOfChecks.end(), temp2.begin(), temp2.end());
		std::vector<std::pair<Vector3, Vector3>> temp3(CheckCollision(node->bottomLeft, dt));
		posOfChecks.insert(posOfChecks.end(), temp3.begin(), temp3.end());
		std::vector<std::pair<Vector3, Vector3>> temp4(CheckCollision(node->bottomRight, dt));
		posOfChecks.insert(posOfChecks.end(), temp4.begin(), temp4.end());

		return posOfChecks;
	}




	for (std::list<EntityBase*>::iterator it = node->entityList.begin(); it != node->entityList.end(); ++it) {

			//GenericEntity* gEntity = dynamic_cast<GenericEntity*>(*it);
			EntityBase* gEntity = dynamic_cast<EntityBase*>(*it);
			GenericEntity* genericEntity = dynamic_cast<GenericEntity*>(*it);
			if (genericEntity)
			{
				if (genericEntity->GetName() == "MONK_HEAD")
				{
					gEntity = genericEntity->rootNode->GetEntity();
				}
			}
			//if (gEntity)
			{
				//== Is scenenode ==//                                     //important param
				std::list<EntityBase*> checkEntities(GetNearbyEntities(gEntity, root, true));
				

				//== Remove the unnessecary obj checks. e.g in the same grid ones and before me ones ==//
				std::list<EntityBase*>::iterator removeIter, checkIter;
				removeIter = node->entityList.begin();
				do {
					if ((checkIter = std::remove(checkEntities.begin(), checkEntities.end(), *removeIter)) != checkEntities.end())
						checkEntities.erase(checkIter);
					
					++removeIter;
					///remove everything incl myself coz idw check with myself
				} while (removeIter != std::next(it, 1) && removeIter != node->entityList.end());
				

				//== Draw a beacon line to indicate that this obj is checking with 2 or more others ==//
				if (checkEntities.size() >= 2)
					posOfChecks.push_back(std::make_pair(gEntity->GetPosition(), Vector3(0, 1000, 0) + gEntity->GetPosition()));

				//== Do the collision check ==//
				for (std::list<EntityBase*>::iterator it2 = checkEntities.begin(); it2 != checkEntities.end(); ++it2) {

					//if (dynamic_cast<EntityBase*>(gEntity))
					CollisionManager::GetInstance()->CheckCollision(gEntity->collider, (*it2)->collider, dt);
					posOfChecks.push_back(std::make_pair((*it)->GetPosition(), (*it2)->GetPosition()));

				}

				
				CExplosive* nade = dynamic_cast<CExplosive*>(gEntity);
				if (nade)
				{
					if (nade->collider->GetMinAABB().y < this->ground->GetPosition().y) {
						nade->SetStatus(false);
					}
				}
			}
			//else
			//{
			//	//the normal thing we always do
			//	std::list<EntityBase*>::iterator it2 = std::next(it, 1);
			//	for (; it2 != node->entityList.end(); ++it2)
			//	{
			//		CollisionManager::GetInstance()->CheckCollision((*it)->collider, (*it2)->collider, dt);
			//		posOfChecks.push_back(std::make_pair((*it)->GetPosition(), (*it2)->GetPosition()));
			//	}
			//}

	}

	return posOfChecks;
}

std::list<EntityBase*> QuadTreeManager::GetNearbyEntities(EntityBase * entity, QuadTree * node, bool addToRender)
{
	//returns including my node de
	std::list<EntityBase*> ret;

	int pointsInside = this->IsAABBInGrid(entity->collider->GetMinAABB(), entity->collider->GetMaxAABB(), node);

	std::vector<QuadTree*> childrens = node->GetAllChildren();
	if (!childrens.empty())
	{
		for (auto child : childrens)
		{
			if (this->IsAABBInGrid(entity->collider->GetMinAABB(), entity->collider->GetMaxAABB(), child) >= 1)
			{
				//one pt inside grid
				std::list<EntityBase*> tempRet(GetNearbyEntities(entity, child, addToRender));
				ret.insert(ret.end(), tempRet.begin(), tempRet.end());
			}
			else
			{

			}
		}
	}
	else
	{
		//std::cout << "IN" << std::endl;
		//at leaf node
		//only here i assign the entities inside the return list
		ret.assign(node->entityList.begin(), node->entityList.end());
		if (addToRender)
		{
			AOEQuad aoequad;
			aoequad.dt = 0.1;
			aoequad.pos = node->position;
			aoequad.scale = node->GetMaxBoundary() - node->GetMinBoundary();

			if (std::find(renderList.begin(), renderList.end(), aoequad) == renderList.end())
			{
				renderList.push_back(aoequad);
				
			}
			//std::cout << "IN" << std::endl;
		}
	}

	return ret;


	///==================== BELOW CODE IS THE LESS OPTIMISED TO GET ENTITY LIST ============================//
	//To use this part, the parameter must not start with root. it should start with its residing node

	//QuadTree* curr = node;
	//while (curr)
	//{
	//	if (this->IsAABBInGrid(entity->GetMinAABB() + entity->GetPosition(), entity->GetMaxAABB() + entity->GetPosition(), curr) == 2 //im totally inside
	//		|| curr->parent == nullptr) // at root
	//	{
	//		while (true)
	//		{

	//			std::vector<QuadTree*> childrens = curr->GetAllChildren();
	//			if (!childrens.empty())
	//			{
	//				//if i can go down one level, i check from thr

	//				for (auto child : childrens)
	//				{
	//					if (this->IsAABBInGrid(entity->GetMinAABB() + entity->GetPosition(), entity->GetMaxAABB() + entity->GetPosition(), child) >= 1)
	//					{
	//						//if at least 1 point inside
	//						ret.insert(ret.end(), child->entityList.begin(), child->entityList.end());
	//					}

	//				}
	//			}
	//			else //at leaf, cant go deeper
	//			{
	//				ret.assign(curr->entityList.begin(), curr->entityList.end());
	//				break;
	//			}
	//		}
	//		break;
	//	}
	//	curr = curr->parent;
	//}

	//return ret;
}

int QuadTreeManager::IsAABBInGrid(Vector3 min, Vector3 max, QuadTree * node)
{
	//min and max must be inside node's min max

	//IMPORTANT NOTE: IGNORING Y FOR NOW
		//+ ( max < node->maxBoundary);


	return(Vector3(min.x, 0, min.z) > node->minBoundary && Vector3(min.x, 0, min.z) < node->maxBoundary) + 
		(Vector3(max.x, 0, min.z) > node->minBoundary && Vector3(max.x, 0, min.z) < node->maxBoundary) +
		(Vector3(max.x, 0, max.z) > node->minBoundary && Vector3(max.x, 0, max.z) < node->maxBoundary) +
		(Vector3(min.x, 0, max.z) > node->minBoundary && Vector3(min.x, 0, max.z) < node->maxBoundary) +

		(Vector3(node->minBoundary.x,0, node->minBoundary.z) > Vector3(min.x, 0, min.z) && Vector3(node->minBoundary.x, 0, node->minBoundary.z) < Vector3(max.x, 0, max.z)) +
		(Vector3(node->maxBoundary.x, 0, node->minBoundary.z) > Vector3(min.x, 0, min.z) && Vector3(node->maxBoundary.x, 0, node->minBoundary.z) < Vector3(max.x, 0, max.z)) +
		(Vector3(node->maxBoundary.x, 0, node->maxBoundary.z) > Vector3(min.x, 0, min.z) && Vector3(node->maxBoundary.x, 0, node->maxBoundary.z) < Vector3(max.x, 0, max.z)) +
		(Vector3(node->minBoundary.x, 0, node->maxBoundary.z) > Vector3(min.x, 0, min.z) && Vector3(node->minBoundary.x, 0, node->maxBoundary.z)  < Vector3(max.x, 0, max.z))
		;
}
