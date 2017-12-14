#include "WeaponInfo.h"
#include "../Projectile/Projectile.h"
#include "../Projectile/Laser/Laser.h"
#include "../QuadTree/QuadTreeManager.h"
#include "EntityBase.h"
#include "../AudioPlayer/AudioPlayer.h"
#include <iostream>
using namespace std;

CWeaponInfo::CWeaponInfo()
	: magRounds(1)
	, maxMagRounds(1)
	, totalRounds(8)
	, maxTotalRounds(8)
	, timeBetweenShots(0.5)
	, elapsedTime(0.0)
	, bFire(true)
{
}


CWeaponInfo::~CWeaponInfo()
{
}

// Set the number of ammunition in the magazine for this player
void CWeaponInfo::SetMagRound(const int magRounds)
{
	this->magRounds = magRounds;
}

// Set the maximum number of ammunition in the magazine for this weapon
void CWeaponInfo::SetMaxMagRound(const int magRounds)
{
	this->magRounds = magRounds;
}

// The current total number of rounds currently carried by this player
void CWeaponInfo::SetTotalRound(const int totalRounds)
{
	this->totalRounds = totalRounds;
}

// The max total number of rounds currently carried by this player
void CWeaponInfo::SetMaxTotalRound(const int maxTotalRounds)
{
	this->maxTotalRounds = maxTotalRounds;
}


// Get the number of ammunition in the magazine for this player
int CWeaponInfo::GetMagRound(void) const
{
	return magRounds;
}

// Get the maximum number of ammunition in the magazine for this weapon
int CWeaponInfo::GetMaxMagRound(void) const
{
	return maxMagRounds;
}

// Get the current total number of rounds currently carried by this player
int CWeaponInfo::GetTotalRound(void) const
{
	return totalRounds;
}

// Get the max total number of rounds currently carried by this player
int CWeaponInfo::GetMaxTotalRound(void) const
{
	return maxTotalRounds;
}

// Set the time between shots
void CWeaponInfo::SetTimeBetweenShots(const double timeBetweenShots)
{
	this->timeBetweenShots = timeBetweenShots;
}

// Set the firing rate in rounds per min
void CWeaponInfo::SetFiringRate(const int firingRate)
{
	timeBetweenShots = 60.0 / (double)firingRate;	// 60 seconds divided by firing rate
}

// Set the firing flag
void CWeaponInfo::SetCanFire(const bool bFire)
{
	this->bFire = bFire;
}

// Get the time between shots
double CWeaponInfo::GetTimeBetweenShots(void) const
{
	return timeBetweenShots;
}

// Get the firing rate
int CWeaponInfo::GetFiringRate(void) const
{
	return (int)(60.0 / timeBetweenShots);	// 60 seconds divided by timeBetweenShots
}

// Get the firing flag
bool CWeaponInfo::GetCanFire(void) const
{
	return bFire;
}

// Initialise this instance to default values
void CWeaponInfo::Init(void)
{
	// The number of ammunition in a magazine for this weapon
	magRounds = 1;
	// The maximum number of ammunition for this magazine for this weapon
	maxMagRounds = 1;
	// The current total number of rounds currently carried by this player
	totalRounds = 8;
	// The max total number of rounds currently carried by this player
	maxTotalRounds = 8;

	// The time between shots
	timeBetweenShots = 0.5;
	// The elapsed time (between shots)
	elapsedTime = 0.0;
	// Boolean flag to indicate if weapon can fire now
	bFire = true;
}

// Update the elapsed time
void CWeaponInfo::Update(const double dt)
{
	elapsedTime += dt;
	if (elapsedTime > timeBetweenShots)
	{
		bFire = true;
		elapsedTime = 0.0;
	}
}

// Discharge this weapon
void CWeaponInfo::Discharge(Vector3 position, Vector3 target, CPlayerInfo* _source)
{
	CAudioPlayer::GetInstance()->GetISoundEngine()->play2D("Audio/SFX/BULLET.wav", false);
	if (bFire)
	{
		// If there is still ammo in the magazine, then fire
		if (magRounds > 0)
		{
			if (hitScan == false)
			{
				// Create a projectile with a cube mesh. Its position and direction is same as the player.
				// It will last for 3.0 seconds and travel at 500 units per second
				CLaser* aProjectile = Create::Laser("RAY",
					position,
					(target - position).Normalized(),
					2.0f,
					10.0f,
					30.f,
					_source);
				aProjectile->SetCollider(true);
			}
			else
			{
				std::deque<EntityBase*> allEntities(QuadTreeManager::GetInstance()->GetAllEntities());

				for (auto e : allEntities)
				{
					Vector3 outPt;

					if (e->rootNode)
					{
						if (isLineIntersectAABB(e, position, (target - position).Normalized(), outPt))
						{
							if (e->isDestroyable ==  true)
								e->SetIsDone(true);
							if (e->isStatic == false)
								e->SetIsDone(true);;
						}

						std::vector<CSceneNode*> children = e->rootNode->GetChildren();
						for (auto child : children)
						{
							if (isLineIntersectAABB(child->GetEntity(), position, (target - position).Normalized(), outPt))
							{
								if (e->isStatic == false || e->isDestroyable == true)
								child->GetEntity()->SetIsDone(true);
							}
						}
					}
					else
						if (isLineIntersectAABB(e, position, (target - position).Normalized(), outPt))
						{
							if (e->isStatic == false)
								e->SetIsDone(true);
						}


					if (outPt == Vector3(0, 0, 0))
						outPt.y = -FLT_MAX;
					QuadTreeManager::AOEQuad aoequad;
					aoequad.pos = outPt;
					aoequad.scale.Set(0.1, 0.1, 0.1);
					aoequad.dt = 2.0;
					QuadTreeManager::GetInstance()->hitScanList.push_back(aoequad);
				}

			}


			//aProjectile->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
			bFire = false;
			//magRounds--;
		}
	}
}

// Reload this weapon
void CWeaponInfo::Reload(void)
{
	if (magRounds < maxMagRounds)
	{
		if (maxMagRounds - magRounds <= totalRounds)
		{
			totalRounds -= maxMagRounds - magRounds;
			magRounds = maxMagRounds;
		}
		else
		{
			magRounds += totalRounds;
			totalRounds = 0;
		}
	}
}

// Add rounds
void CWeaponInfo::AddRounds(const int newRounds)
{
	if (totalRounds + newRounds > maxTotalRounds)
		totalRounds = maxTotalRounds;
	else
		totalRounds += newRounds;
}

// Print Self
void CWeaponInfo::PrintSelf(void)
{
	cout << "CWeaponInfo::PrintSelf()" << endl;
	cout << "========================" << endl;
	cout << "magRounds\t\t:\t" << magRounds << endl;
	cout << "maxMagRounds\t\t:\t" << maxMagRounds << endl;
	cout << "totalRounds\t\t:\t" << totalRounds << endl;
	cout << "maxTotalRounds\t\t:\t" << maxTotalRounds << endl;
	cout << "timeBetweenShots\t:\t" << timeBetweenShots << endl;
	cout << "elapsedTime\t\t:\t" << elapsedTime << endl;
	cout << "bFire\t\t:\t" << bFire << endl;
}











































































































//No, collision is done in CollisionManager, not here ;)
bool CWeaponInfo::isLineIntersectAABB(EntityBase* entity, Vector3& origin, Vector3& dir, Vector3& outNearPoint)
{
	Vector3 MinPt = (entity->collider->GetMinAABB());
	Vector3 MaxPt = (entity->collider->GetMaxAABB());
	dir.Normalize();
	Vector3 invDir(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);

	float lambdaMinX = (MinPt.x - origin.x) * invDir.x;
	float lambdaMaxX = (MaxPt.x - origin.x) * invDir.x;
	float lambdaMinY = (MinPt.y - origin.y) * invDir.y;
	float lambdaMaxY = (MaxPt.y - origin.y) * invDir.y;
	float lambdaMinZ = (MinPt.z - origin.z) * invDir.z;
	float lambdaMaxZ = (MaxPt.z - origin.z) * invDir.z;

	//Rearrange the max and min
	if (lambdaMinX > lambdaMaxX) std::swap(lambdaMinX, lambdaMaxX);
	if (lambdaMinY > lambdaMaxY) std::swap(lambdaMinY, lambdaMaxY);
	if (lambdaMinZ > lambdaMaxZ) std::swap(lambdaMinZ, lambdaMaxZ);

	if ((lambdaMinX > lambdaMaxY) || (lambdaMinY > lambdaMaxX))
		return false;

	if (lambdaMinY > lambdaMinX)
		lambdaMinX = lambdaMinY;

	if (lambdaMaxY < lambdaMaxX)
		lambdaMaxX = lambdaMaxY;

	if ((lambdaMinX > lambdaMaxZ) || (lambdaMinZ > lambdaMaxX))
		return false;
	if (lambdaMinZ > lambdaMinX)
		lambdaMinX = lambdaMinZ;
	if (lambdaMaxZ < lambdaMaxX)
		lambdaMaxX = lambdaMaxZ;

	outNearPoint = origin + (lambdaMinX * dir);
	return true;
}