#pragma once
#include "Vector3.h"
#include "../FPSCamera.h"
#include "../GroundEntity.h"
#include "../WeaponInfo/WeaponInfo.h"
#include "../SpatialPartition/SpatialPartition.h"
#include "Collider\Collider.h"

#include "../Component/Option.h"
#include <map>
#include <functional>
#include <bitset>
#include <conio.h>
#include <set>

class CLuaInterface;
struct lua_State;

class CPlayerInfo : public EntityBase//, public CCollider//, public CSpatialPartition,
{
protected:
	static CPlayerInfo *s_instance;
	CPlayerInfo(void);

public:
	static CPlayerInfo *GetInstance()
	{
		if (!s_instance)
			s_instance = new CPlayerInfo;
		return s_instance;
	}
	static bool DropInstance()
	{
		if (s_instance)
		{
			delete s_instance;
			s_instance = NULL;
			return true;
		}
		return false;
	}
	~CPlayerInfo(void);

	// Initialise this class instance
	void Init(void);
	// Returns true if the player is on ground
	bool isOnGround(void);
	// Returns true if the player is jumping upwards
	bool isJumpUpwards(void);
	// Returns true if the player is on freefall
	bool isFreeFall(void);
	// Set the player's status to free fall mode
	void SetOnFreeFall(bool isOnFreeFall);
	// Set the player to jumping upwards
	void SetToJumpUpwards(bool isOnJumpUpwards);
	// Stop the player's vertical movement
	void StopVerticalMovement(void);
	// Reset this player instance to default
	void Reset(void);

	// Set position
	void SetPos(const Vector3& pos);
	// Set target
	void SetTarget(const Vector3& target);
	// Set Up
	void SetUp(const Vector3& up);
	// Set m_dJumpSpeed of the player
	void SetJumpSpeed(const double m_dJumpSpeed);
	// Set m_dJumpAcceleration of the player
	void SetJumpAcceleration(const double m_dJumpAcceleration);
	// Set Fall Speed of the player
	void SetFallSpeed(const double m_dFallSpeed);
	// Set Fall Acceleration of the player
	void SetFallAcceleration(const double m_dFallAcceleration);
	// Set the boundary for the player info
	void SetBoundary(Vector3 max, Vector3 min);
	// Set the terrain for the player info
	void SetTerrain(GroundEntity* m_pTerrain);

	// Get position
	Vector3 GetPos(void) const;
	// Get target
	Vector3 GetTarget(void) const;
	// Get Up
	Vector3 GetUp(void) const;
	// Get Jump Speed of the player
	double GetJumpSpeed(void) const;
	// Get Jump Acceleration of the player
	double GetJumpAcceleration(void) const;
	// Get Fall Speed of the player
	double GetFallSpeed(void) const;
	// Get Fall Acceleration of the player
	double GetFallAcceleration(void) const;

	// Update Jump Upwards
	void UpdateJumpUpwards(double dt = 0.0333f);
	// Update FreeFall
	void UpdateFreeFall(double dt = 0.0333f);
	// Update
	void Update(double dt = 0.0333f);

	// Constrain the position within the borders
	void Constrain(void);

	// Handling Camera
	void AttachCamera(FPSCamera* _cameraPtr);
	void DetachCamera();

	/*Set updated spatial partition object.*/
	void SetSpatialPartition(CSpatialPartition* _spatitionPartition) { spatialPartition = _spatitionPartition; }
	/*Get the spatial partition object to optimise collision check.*/
	CSpatialPartition* GetSpatialPartition(void) { return spatialPartition; }

	/*For checking if hitscan is on.*/
	CWeaponInfo* GetPrimaryWeapon(void) { return primaryWeapon; }

	void SetOptimiseUpdate(bool _optimiseUpdate) { optimiseUpdate = _optimiseUpdate; }
	const bool* GetOptimiseUpdate(void) { return &optimiseUpdate; }

	std::map<int*, std::pair<std::string, std::function<void(float)>>>&GetBindKeyMap(void) { return bindKeyMap; }
private:
	Vector3 defaultPosition, defaultTarget, defaultUp;
	Vector3 position, target, up;
	Vector3 maxBoundary, minBoundary;
	GroundEntity* m_pTerrain;

	double m_dSpeed;
	double m_dAcceleration;

	bool m_bJumpUpwards;
	double m_dJumpSpeed;
	double m_dJumpAcceleration;

	double m_dFallSpeed;
	bool m_bFallDownwards;
	double m_dFallAcceleration;

	double m_dElapsedTime;

	FPSCamera* attachedCamera;

	CWeaponInfo* primaryWeapon;
	CWeaponInfo* secondaryWeapon;

	/*Spatial Partitioning Info.*/
	CSpatialPartition* spatialPartition;

	int* keyMoveForward;
	int* keyMoveBackward;
	int* keyMoveLeft;
	int* keyMoveRight;
	int* keyMoveJump;
	int* keyReload;
	int* keyHitscan;
	int* keyFire;
	int* keyBomb;
	int* keyReset;

	/*For converting int (input) to memory address for accessing bindKeyMap.*/
	std::map<int, int*>intToMemMap;
	/*Binding the key to their respective function to reduce if statement(s).
	The pair consists of the variable name for storing into lua and the function pointer to execute the respective function based on button input.*/
	std::map<int*, std::pair<std::string, std::function<void(float)>>>bindKeyMap;
	/*For checking if the button is key pressed, key down or key up.*/
	std::map<int*, std::function<bool(int)>>isKeyMap;
	//std::function<void(std::string*)> bindKeyMap;
	OptionBase<Key>keyCheck;
	CLuaInterface* luaInterface;
	lua_State* luaState;
	bool optimiseUpdate;
};
