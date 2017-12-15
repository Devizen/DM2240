#include "ScoreManager.h"

#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../Application.h"
#include "MeshBuilder.h"
#include <sstream>


CScoreManager* CScoreManager::s_instance = 0;

CScoreManager::CScoreManager(void)
{
}

CScoreManager::~CScoreManager(void)
{
}

CScoreManager * CScoreManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CScoreManager();
	return s_instance;
}

void CScoreManager::RenderUI()
{
	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	std::stringstream ss;
	ss << std::fixed;
	ss.precision(2);
	ss << "Score: " << score;

	ms.PushMatrix();

	ms.Translate(0.15 * Application::GetInstance().GetWindowWidth() * 0.5f, 0.9 * Application::GetInstance().GetWindowHeight() * 0.5f, 0);
	ms.Scale(Vector3(25, 25, 10));
	RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("text"), ss.str(), Color(1, 0, 0));

	ms.PopMatrix();
}
