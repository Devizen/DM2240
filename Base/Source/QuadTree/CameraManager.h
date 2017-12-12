#pragma once

#include "../FPSCamera.h"

class CameraManager
{
	CameraManager();
	~CameraManager();

	static CameraManager* instance;

	FPSCamera* playerCam;
	FPSCamera* birdEyeCam;
public:
	static CameraManager* GetInstance() { return (instance ? instance : instance = new CameraManager()); }

	FPSCamera* AttachPlayerCam(FPSCamera* cam) { this->playerCam = cam; return this->playerCam; }

	FPSCamera* GetPlayerCam() { return playerCam; }
	FPSCamera* GetBirdEyeCam() { return birdEyeCam; }

	void Destroy();
};