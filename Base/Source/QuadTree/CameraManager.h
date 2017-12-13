#pragma once

#include "../FPSCamera.h"

class CameraManager
{
	CameraManager();
	~CameraManager();

	static CameraManager* instance;

	FPSCamera* playerCam;
	FPSCamera* birdEyeCam;

	struct Frustum
	{
		double fovAngle, aspectRatio, nearPlane, farPlane;
	} frustum;
public:
	static CameraManager* GetInstance() { return (instance ? instance : instance = new CameraManager()); }

	FPSCamera* AttachPlayerCam(FPSCamera* cam) { this->playerCam = cam; return this->playerCam; }

	FPSCamera* GetPlayerCam() { return playerCam; }
	FPSCamera* GetBirdEyeCam() { return birdEyeCam; }
	FPSCamera* GetActiveCam() { return (toggleBirdEyeView ? birdEyeCam : playerCam); }

	//void Destroy();

	bool toggleBirdEyeView;

	void RenderPlayerFrustum();

	double GetFrustumFoV() { return frustum.fovAngle; }
	double GetFrustumAspect() { return frustum.aspectRatio; }
	double GetNearPlane() { return frustum.nearPlane; }
	double GetFarPlane() { return frustum.farPlane; }

	bool IsAABBInFrustum(Vector3 min, Vector3 max);
};