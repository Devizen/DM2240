#include "CameraManager.h"
#include "../FPSCamera.h"
#include "RenderHelper.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager::CameraManager() : playerCam (nullptr), birdEyeCam(nullptr)
{
	birdEyeCam = new FPSCamera();
	toggleBirdEyeView = false;
}

CameraManager::~CameraManager()
{
	delete birdEyeCam;
}

//void CameraManager::Destroy()
//{
//}

void CameraManager::RenderPlayerFrustum()
{

}
