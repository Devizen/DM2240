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

}

bool CameraManager::IsAABBInFrustum(Vector3 min, Vector3 max)
{
	Vector3 boundingPoints[8];
	boundingPoints[0] = Vector3(min);
	boundingPoints[1] = Vector3(min.x, min.y, max.z);
	boundingPoints[2] = Vector3(max.x, min.y, max.z);
	boundingPoints[3] = Vector3(max.x, min.y, min.z);

	boundingPoints[4] = Vector3(min.x, max.y, min.z);
	boundingPoints[5] = Vector3(min.x, max.y, max.z);
	boundingPoints[6] = Vector3(max.x, max.y, max.z);
	boundingPoints[7] = Vector3(max.x, max.y, min.z);

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

	Vector3 frustumPlanes[6];
	// left right up down
	frustumPlanes[0] = rotateAboutUp * forward;
	frustumPlanes[1] = rotateAboutDown * forward;
	frustumPlanes[2] = rotateAboutRight * forward;
	frustumPlanes[3] = rotateAboutLeft * forward;
	//near far
	frustumPlanes[4] = camRight;
	frustumPlanes[5] = camRight;

	double radianFov = Math::DegreeToRadian(frustum.fovAngle * 0.5f);
	double cosFov = cos(radianFov);
	double nearHypotenuse = frustum.nearPlane / cosFov;
	double farHypotenuse = frustum.farPlane / cosFov;
	for (auto bp : boundingPoints)
	{
		bool insideFrustum = false;
		//cast on the plane, see if it is inside the length
		// ( a . b^ ) b^   vector projection
		{
			//left plane
			//Vector3 planeNormalized = frustumPlanes[0].Normalized();
			//Vector3 pointRelativeToCam = -playerCam->GetCameraPos() + bp;
			//Vector3 pointProjection = pointRelativeToCam.Dot(planeNormalized) * planeNormalized;


			//double NormDotResult = pointRelativeToCam.Normalized().Dot(planeNormalized);
			//if (NormDotResult > cosFov && pointProjection.Length() > nearHypotenuse && pointProjection.Length() < farHypotenuse)
			//{
			//	//a.b = cos(0)
			//	//normdotresult must be more than the cos of 45 degree
			//	//more coz of the cos curve
			//	insideFrustum = true;
			//}
		}
		{
			for (int i = 0; i < 4; ++i)
			{
				Vector3 planeNormalized = frustumPlanes[i].Normalized();
				Vector3 pointRelativeToCam = -playerCam->GetCameraPos() + bp;
				Vector3 pointProjection = pointRelativeToCam.Dot(planeNormalized) * planeNormalized;


				double NormDotResult = pointRelativeToCam.Normalized().Dot(planeNormalized);
				if (NormDotResult > cosFov && pointProjection.Length() > nearHypotenuse && pointProjection.Length() < farHypotenuse)
					insideFrustum = true;
			}

		}
		{

		}




		if (insideFrustum == true)
		{
			//after all checks and all valid
			return true;
		}
	}


	return false;
}
