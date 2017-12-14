#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

class CScoreManager
{
	static CScoreManager* s_instance;
public:
	CScoreManager(void);
	~CScoreManager(void);
	static CScoreManager* GetInstance(void);

	void AddScore(float _score) { score += _score; }
	void SetScore(float _score) { score = _score; }
	float GetScore(void) { return score; }
	void ResetScore(void) { score = 0.f; }
private:
	float score;
protected:
};

#endif