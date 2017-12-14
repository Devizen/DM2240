#include "ScoreManager.h"

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
