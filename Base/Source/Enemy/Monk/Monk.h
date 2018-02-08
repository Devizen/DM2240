#ifndef MONK_H
#define MONK_H
#include "../../GenericEntity.h"
#include "Vector3.h"
#include <vector>
#include "../../PlayerInfo/PlayerInfo.h"
#include "../../Waypoint/Waypoint.h"

class CMonk : public GenericEntity{
public:
	enum STATES
	{
		PATROL,
		CHASE,
		//ALL_CHASE
	};

	CMonk();
	~CMonk();

	void Init(void);
	void SetModelMeshes(Mesh* _head, Mesh* _body, Mesh* _leftArm, Mesh* _rightArm, Mesh* _leftLeg, Mesh* _rightLeg);
	GenericEntity* GetEntityPart(std::string _part);
	EntityBase* GetCore();
	void SetEntityPart(std::string _part, GenericEntity* _entity);

	void UpdatePart(double dt = 0.0, std::string _part = "");

	/*Add to Part List.*/
	void AddPartToList(GenericEntity* _part) { partList.push_back(_part); }
	/*Get Part List for iteration.*/
	std::vector<GenericEntity*>&GetPartList(void) { return partList; }

	/*Set player object.*/
	void SetPlayer(CPlayerInfo* _player) { player = _player; }
	/*Get player object.*/
	CPlayerInfo* GetPlayer() { return player; }

	virtual void CollisionResponse(EntityBase* other);

	int GetPartSize() { return partList.size(); }
	//EntityBase* root;

	/*Waypoints.*/
	/*Get the next waypoint.*/
	CWaypoint* GetNextWaypoint(void);
	/*Vector containing IDs of waypoints.*/
	std::vector<int>listOfWaypoints;
	/*Current ID of Waypoint.*/
	int m_iWaypointIndex;
	double m_speed;
	/*Waypoint ID*/
	int currWaypointID, nextWaypointID;

	STATES GetState() { return currState; }
private:
	float moveSpeed;

	Mesh* headModelMesh;
	Mesh* bodyModelMesh;
	Mesh* leftArmModelMesh;
	Mesh* rightArmModelMesh;
	Mesh* leftLegModelMesh;
	Mesh* rightLegModelMesh;


	GenericEntity* head;
	GenericEntity* body;
	GenericEntity* leftArm;
	GenericEntity* rightArm;
	GenericEntity* leftLeg;
	GenericEntity* rightLeg;

	/*Consists of all body parts.*/
	std::vector<GenericEntity*>partList;

	/*Player.*/
	CPlayerInfo* player;

	/*States*/
	STATES currState;
	void UpdateState(double dt);
protected:
};

namespace Create
{
	CMonk* Monk(const Vector3 & _position, const Vector3 & _scale, CPlayerInfo* _player = nullptr);
};
#endif