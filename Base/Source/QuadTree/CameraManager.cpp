#include "CameraManager.h"
#include "../FPSCamera.h"
#include "RenderHelper.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager::CameraManager() : playerCam (nullptr), birdEyeCam(nullptr)
{
	birdEyeCam = new FPSCamera();
	toggleBirdEyeView = false;

	frustum.fovAngle = 45.0;
	frustum.aspectRatio = 4.0 / 3.0;
	frustum.nearPlane = 0.1;
	frustum.farPlane = 1000.0;
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
	//len of perpendicular farplane
	//tan(angle) = oppo / adjacent
	//double lenOfPerFar = frustum.farPlane * tan(Math::DegreeToRadian(frustum.fovAngle));

	// c = sqrt(a2 + b2)
	//double edgeLen = sqrt(frustum.farPlane * frustum.farPlane + lenOfPerFar * lenOfPerFar);
	//line top left
	Vector3 forward = (playerCam->GetCameraTarget() - playerCam->GetCameraPos()).Normalize();
	
	Mtx44 rotateAboutUp;
	rotateAboutUp.SetToRotation(frustum.fovAngle * 0.5f, playerCam->GetCameraUp().x, playerCam->GetCameraUp().y, playerCam->GetCameraUp().z);
	Mtx44 rotateAboutDown;
	rotateAboutDown.SetToRotation(frustum.fovAngle * 0.5f, -playerCam->GetCameraUp().x, -playerCam->GetCameraUp().y, -playerCam->GetCameraUp().z);
	Vector3 camRight = (playerCam->GetCameraTarget() - playerCam->GetCameraPos()).Normalize().Cross(playerCam->GetCameraUp()).Normalize();
	Mtx44 rotateAboutRight;
	rotateAboutRight.SetToRotation(frustum.fovAngle * 0.5f, camRight.x, camRight.y, camRight.z);
	Mtx44 rotateAboutLeft;
	rotateAboutLeft.SetToRotation(frustum.fovAngle * 0.5f, -camRight.x, -camRight.y, -camRight.z);

	//1 tl, 2 tr, 3 bl, 4 br
	Vector3 line1 = forward;
	Vector3 line2 = forward;
	Vector3 line3 = forward;
	Vector3 line4 = forward;
	line1 = rotateAboutUp * line1;
	line1 = rotateAboutRight * line1;

	line2 = rotateAboutDown * line2;
	line2 = rotateAboutRight * line2;

	line3 = rotateAboutUp * line3;
	line3 = rotateAboutLeft * line3;

	line4 = rotateAboutDown * line4;
	line4 = rotateAboutLeft * line4;

	std::vector<Vector3> linesToRender;
	linesToRender.push_back(playerCam->GetCameraPos() + line1 * frustum.nearPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line1 * frustum.farPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line2 * frustum.nearPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line2 * frustum.farPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line3 * frustum.nearPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line3 * frustum.farPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line4 * frustum.nearPlane);
	linesToRender.push_back(playerCam->GetCameraPos() + line4 * frustum.farPlane);

	RenderHelper::DrawLine(linesToRender, Color(0, 0, 1), 2);

	//RenderHelper::DrawLine(playerCam->GetCameraPos() + line1 * frustum.nearPlane,
	//	playerCam->GetCameraPos() + line1 * frustum.farPlane, Color(0, 0, 1));
	//RenderHelper::DrawLine(playerCam->GetCameraPos() + line2 * frustum.nearPlane,
	//	playerCam->GetCameraPos() + line2 * frustum.farPlane, Color(0, 0, 1));
	//RenderHelper::DrawLine(playerCam->GetCameraPos() + line3 * frustum.nearPlane,
	//	playerCam->GetCameraPos() + line3 * frustum.farPlane, Color(0, 0, 1));
	//RenderHelper::DrawLine(playerCam->GetCameraPos() + line4 * frustum.nearPlane,
	//	playerCam->GetCameraPos() + line4 * frustum.farPlane, Color(0, 0, 1));
}
