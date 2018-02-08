#include "PlayerInfo.h"
#include <iostream>

#include "MouseController.h"
#include "KeyboardController.h"
#include "Mtx44.h"
#include "../Projectile/Projectile.h"
#include "../WeaponInfo/Pistol.h"
#include "../Projectile/Explosive.h"
#include "../QuadTree/CameraManager.h"
#include "LuaInterface.h"

// Allocating and initializing CPlayerInfo's static data member.  
// The pointer is allocated but not the object's constructor.
CPlayerInfo *CPlayerInfo::s_instance = 0;

CPlayerInfo::CPlayerInfo(void)
	: m_dSpeed(40.0)
	, m_dAcceleration(10.0)
	, m_bJumpUpwards(false)
	, m_dJumpSpeed(10.0)
	, m_dJumpAcceleration(-10.0)
	, m_bFallDownwards(false)
	, m_dFallSpeed(0.0)
	, m_dFallAcceleration(-10.0)
	, attachedCamera(NULL)
	, m_pTerrain(NULL)
	, primaryWeapon(NULL)
	, secondaryWeapon(NULL)
	, spatialPartition(nullptr)
	, keyMoveForward(nullptr)
	, keyMoveBackward(nullptr)
	, keyMoveLeft(nullptr)
	, keyMoveRight(nullptr)
	, keyMoveJump(nullptr)
	, keyReload(nullptr)
	, keyHitscan(nullptr)
	, keyFire(nullptr)
	, keyBomb(nullptr)
	, keyReset(nullptr)
	, luaInterface(nullptr)
	, luaState(nullptr)
	, optimiseUpdate(false)
{
}

CPlayerInfo::~CPlayerInfo(void)
{
	if (secondaryWeapon)
	{
		delete secondaryWeapon;
		secondaryWeapon = NULL;
	}
	if (primaryWeapon)
	{
		delete primaryWeapon;
		primaryWeapon = NULL;
	}
	m_pTerrain = NULL;

	while (!bindKeyMap.empty())
	{
		int* temp = bindKeyMap.begin()->first;
		delete temp;
		temp = nullptr;
		bindKeyMap.erase(bindKeyMap.begin());
	}
	keyMoveForward = nullptr;
	keyMoveBackward = nullptr;
	keyMoveLeft = nullptr;
	keyMoveRight = nullptr;
	keyMoveJump = nullptr;
	keyReload = nullptr;
	keyHitscan = nullptr;
	keyFire = nullptr;
	keyBomb = nullptr;
	keyReset = nullptr;

	optimiseUpdate = false;
}

// Initialise this class instance
void CPlayerInfo::Init(void)
{
	// Set the default values
	defaultPosition.Set(0,0,10);
	defaultTarget.Set(0,0,0);
	defaultUp.Set(0,1,0);

	// Set the current values
	position.Set(0, 0, 10);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);

	// Set Boundary
	maxBoundary.Set(1,1,1);
	minBoundary.Set(-1, -1, -1);

	// Set the pistol as the primary weapon
	primaryWeapon = new CPistol();
	primaryWeapon->Init();

	// Set Collider
	this->collider = new CCollider(this);
	this->collider->SetMinAABB(Vector3(-0.5f, 0));
	this->collider->SetMaxAABB(Vector3(0.5f, 2));
	//this->minAABB.Set(-0.5f, 0);
	//this->maxAABB.Set(0.5f, 2);

	/*Initialise key with Lua.*/
	luaInterface = CLuaInterface::GetInstance();
	luaState = luaInterface->luaState;
	luaL_dofile(luaState, "Lua//Keyboard.lua");

	/*Move Forward.*/
	bindKeyMap[new int()];
	/*Move Backward.*/
	bindKeyMap[new int()];
	/*Move Left.*/
	bindKeyMap[new int()];
	/*Move Right.*/
	bindKeyMap[new int()];
	/*Jump.*/
	bindKeyMap[new int()];
	/*Reload.*/
	bindKeyMap[new int()];
	/*Hitscan Toggle.*/
	bindKeyMap[new int()];
	/*Fire.*/
	bindKeyMap[new int()];
	/*Bomb.*/
	bindKeyMap[new int()];
	/*Reset.*/
	bindKeyMap[new int()];

	unsigned int index = 0;

	for (std::map<int*, std::pair<std::string, std::function<void(float)>>>::iterator it = bindKeyMap.begin(); it != bindKeyMap.end(); ++it, ++index)
	{
		switch (index)
		{
		case 0:
			keyMoveForward = (*it).first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyDown(_input);
			};
			it->second.first = "keyMoveForward";
			it->second.second = [this](float _dt)->void
			{
				Vector3 viewVector = target - position;
				Vector3 rightUV;
				Vector3 temp(viewVector);
				temp.y = 0;
				position += temp.Normalized() * (float)m_dSpeed * (float)_dt * CameraManager::GetInstance()->speedUp;
				// Constrain the position
				Constrain();
				// Update the target
				target = position + viewVector;
			};
			break;
		case 1:
			keyMoveBackward = (*it).first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyDown(_input);
			};
			it->second.first = "keyMoveBackward";
			it->second.second = [this](float _dt)->void
			{
				Vector3 viewVector = target - position;
				Vector3 rightUV;
				Vector3 temp(viewVector);
				temp.y = 0;
				position -= temp.Normalized() * (float)m_dSpeed * (float)_dt * CameraManager::GetInstance()->speedUp;
				// Constrain the position
				Constrain();
				// Update the target
				target = position + viewVector;
			};
			break;
		case 2:
			keyMoveLeft = (*it).first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyDown(_input);
			};
			it->second.first = "keyMoveLeft";
			it->second.second = [this](float _dt)->void
			{
				Vector3 viewVector = target - position;
				Vector3 rightUV;
				rightUV = (viewVector.Normalized()).Cross(up);
				rightUV.y = 0;
				rightUV.Normalize();
				position -= rightUV * (float)m_dSpeed * (float)_dt * CameraManager::GetInstance()->speedUp;
				// Constrain the position
				Constrain();
				// Update the target
				target = position + viewVector;
			};
			break;
		case 3:
			keyMoveRight = (*it).first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyDown(_input);
			};
			it->second.first = "keyMoveRight";
			it->second.second = [this](float _dt)->void
			{
				Vector3 viewVector = target - position;
				Vector3 rightUV;
				rightUV = (viewVector.Normalized()).Cross(up);
				rightUV.y = 0;
				rightUV.Normalize();
				position += rightUV * (float)m_dSpeed * (float)_dt * CameraManager::GetInstance()->speedUp;
				// Constrain the position
				Constrain();
				// Update the target
				target = position + viewVector;
			};
			break;
		case 4:
			keyMoveJump = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyPressed(_input);
			};
			it->second.first = "keyMoveJump";
			it->second.second = [this](float _dt)->void
			{
				SetToJumpUpwards(true);
			};
			break;
		case 5:
			keyReload = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyPressed(_input);
			};
			it->second.first = "keyReload";
			it->second.second = [this](float _dt)->void
			{
				if (primaryWeapon)
				{
					primaryWeapon->Reload();
#ifdef _DEBUG
					primaryWeapon->PrintSelf();
#endif
				}
			};
			break;
		case 6:
			keyHitscan = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyPressed(_input);
			};
			it->second.first = "keyHitscan";
			it->second.second = [this](float _dt)->void
			{
				primaryWeapon->hitScan = (primaryWeapon->hitScan ? false : true);
			};
			break;
		case 7:
			keyFire = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return MouseController::GetInstance()->IsButtonDown(_input);
			};
			it->second.first = "keyFire";
			it->second.second = [this](float _dt)->void
			{
				if (primaryWeapon)
					primaryWeapon->Discharge(position, target, this);
			};
			break;
		case 8:
			keyBomb = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return MouseController::GetInstance()->IsButtonDown(_input);
			};
			it->second.first = "keyBomb";
			it->second.second = [this](float _dt)->void
			{
				CExplosive* nade = Create::Explosive("NADE", position, (target - position).Normalize(), 5.0, 75.0, 1, this);
			};
			break;
		case 9:
			keyReset = it->first;
			isKeyMap[it->first] = [this](unsigned char _input)->bool
			{
				return KeyboardController::GetInstance()->IsKeyPressed(_input);
			};
			it->second.first = "keyReset";
			it->second.second = [this](float _dt)->void
			{
				Reset();
			};
			break;
		}
	}

	/*Movement.*/
	*keyMoveForward = luaInterface->GetIntValue("keyMoveForward");
	intToMemMap[(*keyMoveForward)] = keyMoveForward;
	*keyMoveBackward = luaInterface->GetIntValue("keyMoveBackward");
	intToMemMap[(*keyMoveBackward)] = keyMoveBackward;
	*keyMoveLeft = luaInterface->GetIntValue("keyMoveLeft");
	intToMemMap[(*keyMoveLeft)] = keyMoveLeft;
	*keyMoveRight = luaInterface->GetIntValue("keyMoveRight");
	intToMemMap[(*keyMoveRight)] = keyMoveRight;

	*keyMoveJump = luaInterface->GetIntValue("keyMoveJump");
	intToMemMap[(*keyMoveJump)] = keyMoveJump;

	*keyReload = luaInterface->GetIntValue("keyReload");
	intToMemMap[(*keyReload)] = keyReload;

	*keyHitscan = luaInterface->GetIntValue("keyHitscan");
	intToMemMap[(*keyHitscan)] = keyHitscan;

	*keyFire = luaInterface->GetIntValue("keyFire");
	intToMemMap[(*keyFire)] = keyFire;

	*keyBomb = luaInterface->GetIntValue("keyBomb");
	intToMemMap[(*keyBomb)] = keyBomb;

	*keyReset = luaInterface->GetIntValue("keyReset");
	intToMemMap[(*keyReset)] = keyReset;
}

// Returns true if the player is on ground
bool CPlayerInfo::isOnGround(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is jumping upwards
bool CPlayerInfo::isJumpUpwards(void)
{
	if (m_bJumpUpwards == true && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is on freefall
bool CPlayerInfo::isFreeFall(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == true)
		return true;

	return false;
}

// Set the player's status to free fall mode
void CPlayerInfo::SetOnFreeFall(bool isOnFreeFall)
{
	if (isOnFreeFall == true)
	{
		m_bJumpUpwards = false;
		m_bFallDownwards = true;
		m_dFallSpeed = 0.0;
	}
}

// Set the player to jumping upwards
void CPlayerInfo::SetToJumpUpwards(bool isOnJumpUpwards)
{
	if (isOnJumpUpwards == true)
	{
		m_bJumpUpwards = true;
		m_bFallDownwards = false;
		m_dJumpSpeed = 15.0;
	}
}

// Set position
void CPlayerInfo::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CPlayerInfo::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set position
void CPlayerInfo::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set m_dJumpAcceleration of the player
void CPlayerInfo::SetJumpAcceleration(const double m_dJumpAcceleration)
{
	this->m_dJumpAcceleration = m_dJumpAcceleration;
}

// Set Fall Acceleration of the player
void CPlayerInfo::SetFallAcceleration(const double m_dFallAcceleration)
{
	this->m_dFallAcceleration = m_dFallAcceleration;
}

// Set the boundary for the player info
void CPlayerInfo::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player info
void CPlayerInfo::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != NULL)
	{
		this->m_pTerrain = m_pTerrain;

		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Stop the player's movement
void CPlayerInfo::StopVerticalMovement(void)
{
	m_bJumpUpwards = false;
	m_bFallDownwards = false;
}

// Reset this player instance to default
void CPlayerInfo::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	// Stop vertical movement too
	StopVerticalMovement();
}

// Get position x of the player
Vector3 CPlayerInfo::GetPos(void) const
{
	return position;
}

// Get target
Vector3 CPlayerInfo::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CPlayerInfo::GetUp(void) const
{
	return up;
}

// Get m_dJumpAcceleration of the player
double CPlayerInfo::GetJumpAcceleration(void) const
{
	return m_dJumpAcceleration;
}

// Update Jump Upwards
void CPlayerInfo::UpdateJumpUpwards(double dt)
{
	//if (m_bJumpUpwards == false)
	//	return;

	//// Update the jump's elapsed time
	//m_dElapsedTime += dt;

	//// Update position and target y values
	//// Use SUVAT equation to update the change in position and target
	//// s = u * t + 0.5 * a * t ^ 2
	//position.y += (float)(m_dJumpSpeed * m_dElapsedTime + 0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	//target.y += (float)(m_dJumpSpeed*m_dElapsedTime + 0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	//// Use this equation to calculate final velocity, v
	//// SUVAT: v = u + a * t;
	//// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	//m_dJumpSpeed = m_dJumpSpeed + m_dJumpAcceleration * m_dElapsedTime;
	//// Check if the jump speed is less than zero, then it should be falling
	//if (m_dJumpSpeed < 0.0)
	//{
	//	m_dJumpSpeed = 0.0;
	//	m_bJumpUpwards = false;
	//	m_dFallSpeed = 0.0;
	//	m_bFallDownwards = true;
	//	m_dElapsedTime = 0.0;
	//}

	if (m_bJumpUpwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dJumpSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dJumpSpeed*dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dJumpSpeed = m_dJumpSpeed + m_dJumpAcceleration * dt;
	// Check if the jump speed is less than zero, then it should be falling
	if (m_dJumpSpeed < 0.0)
	{
		m_dJumpSpeed = 0.0;
		m_bJumpUpwards = false;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = true;
	}
}

// Update FreeFall
void CPlayerInfo::UpdateFreeFall(double dt)
{
	if (m_bFallDownwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dFallSpeed = m_dFallSpeed + m_dFallAcceleration * dt;
	// Check if the jump speed is below terrain, then it should be reset to terrain height
	if (position.y < m_pTerrain->GetTerrainHeight(position))
	{
		Vector3 viewDirection = target - position;
		position.y = m_pTerrain->GetTerrainHeight(position);
		target = position + viewDirection;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = false;
	}
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void CPlayerInfo::Update(double dt)
{	
	static int iterateCount = 0;
	/*Handles all the keyboard controls; type of key (press, down or up) and functionality.*/

	switch (optimiseUpdate)
	{
	case false:
	{
		/*Not optimised as it loop through the entire key map.
		Uncomment this to see how much iteration(s) it does for each frame.*/
		for (std::map<int*, std::pair<std::string, std::function<void(float)>>>::iterator it = bindKeyMap.begin(); it != bindKeyMap.end(); ++it)
		{
			/*Check if it is key press, down or up.*/
			if (isKeyMap[it->first]((*(it->first))))
				/*Call the function pointer binded to the key.*/
				bindKeyMap[it->first].second((static_cast<float>(dt)));

#ifdef _DEBUG
			std::cout << "<Looping> " << std::to_string(iterateCount) << std::endl;
			++iterateCount;
#endif
		}
		break;
	}
	case true:
	{
		/*Optimised version, enter the respective call based on input callback.
		Eliminated looping through the keyMap and if statement.*/
		static std::queue<int>*keyInput = KeyboardController::GetInstance()->GetKeyInput();
		static std::set<int>pressedKey;
		while (!keyInput->empty())
		{
			if (intToMemMap[keyInput->front()])
			{
#ifdef _DEBUG
				std::cout << "<Input exist in map.>" << std::endl;
				std::cout << "<Adding to pressedKey.>" << std::to_string(iterateCount) << std::endl;
				++iterateCount;
#endif
				pressedKey.insert(keyInput->front());
			}
#ifdef _DEBUG
			else
			{
				std::cout << "<Input does not exist in map.>" << std::to_string(iterateCount) << std::endl;
				++iterateCount;
			}
#endif
			keyInput->pop();
		}

		if (!pressedKey.empty())
		{
#ifdef _DEBUG
			std::cout << "<Starting to iterate through pressedKey.> " << std::to_string(iterateCount) << std::endl;
			++iterateCount;
#endif
			for (std::set<int>::iterator it = pressedKey.begin(); it != pressedKey.end();)
			{
				/*Check if it is key press, down or up.*/
				if (isKeyMap[intToMemMap[(*it)]]((*(intToMemMap[(*it)]))))
				{
					/*Call the function pointer binded to the key.*/
					bindKeyMap[intToMemMap[(*it)]].second((static_cast<float>(dt)));
#ifdef _DEBUG
					std::cout << "<Pressing \"" << std::to_string((*it)) << "\" key.> " << std::to_string(iterateCount) << std::endl;
					++iterateCount;
#endif
				}
				/*Remove from pressedKey if there was no input to prevent it from unwanted loop check.*/
				else
				{
#ifdef _DEBUG
					std::cout << "<Not pressing \"" << std::to_string((*it)) << "\" key, removing key from pressedKey set.> " << std::to_string(iterateCount) << std::endl;
					++iterateCount;
#endif
					it = pressedKey.erase(it);
					continue;
				}
				++it;
			}
		}
#ifdef _DEBUG
		else
		{
			std::cout << "<Nothing was pressed for this frame.> " << std::to_string(iterateCount) << std::endl;
			++iterateCount;
		}
#endif
		break;
	}
	}

	double mouse_diff_x, mouse_diff_y;
	MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	double camera_yaw = mouse_diff_x * 0.0174555555555556;		// 3.142 / 180.0
	double camera_pitch = mouse_diff_y * 0.0174555555555556;	// 3.142 / 180.0

	//Update the camera direction based on mouse move
	{
		Vector3 viewUV = (target - position).Normalized();
		Vector3 rightUV;

		{
			float yaw = (float)(-m_dSpeed * 2.0f *camera_yaw * (float)dt);
			Mtx44 rotation;
			rotation.SetToRotation(yaw, 0, 1, 0);
			viewUV = rotation * viewUV;
			target = position + viewUV;
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
		}
		{
			float pitch = (float)(-m_dSpeed * 2.0f * camera_pitch * (float)dt);
			rightUV = viewUV.Cross(up);
			rightUV.y = 0;
			rightUV.Normalize();
			up = rightUV.Cross(viewUV).Normalized();
			Mtx44 rotation;
			rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
			viewUV = rotation * viewUV;
			target = position + viewUV;
		}
	}

	if (primaryWeapon)
		primaryWeapon->Update(dt);
	if (secondaryWeapon)
		secondaryWeapon->Update(dt);

	UpdateJumpUpwards(dt);
	UpdateFreeFall(dt);
	
	// If a camera is attached to this playerInfo class, then update it
	if (attachedCamera)
	{
		attachedCamera->SetCameraPos(position);
		attachedCamera->SetCameraTarget(target);
		attachedCamera->SetCameraUp(up);
	}
}

// Constrain the position within the borders
void CPlayerInfo::Constrain(void)
{
	// Constrain player within the boundary
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	//if (position.y > maxBoundary.y - 1.0f)
	//	position.y = maxBoundary.y - 1.0f;
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	//if (position.y < minBoundary.y + 1.0f)
	//	position.y = minBoundary.y + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the player is not jumping nor falling, then adjust his y position
	if ((m_bJumpUpwards == false) && (m_bFallDownwards == false))
	{
		// if the y position is not equal to terrain height at that position, 
		// then update y position to the terrain height
		if (position.y != m_pTerrain->GetTerrainHeight(position))
			position.y = m_pTerrain->GetTerrainHeight(position);
	}
}

void CPlayerInfo::AttachCamera(FPSCamera* _cameraPtr)
{
	attachedCamera = _cameraPtr;
}

void CPlayerInfo::DetachCamera()
{
	attachedCamera = nullptr;
}
