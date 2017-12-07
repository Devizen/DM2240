#ifndef LEVELOFDETAIL_H
#define LEVELOFDETAIL_H
#include "Mesh.h"
class CLevelOfDetail
{
public:
	enum LEVEL {
		NONE = 0,
		LOW,
		MEDIUM,
		HIGH,
		TOTAL
	};

	CLevelOfDetail(void);
	~CLevelOfDetail(void);
	bool Init(const std::string& _meshNameHigh,
		const std::string& _meshNameMedium,
		const std::string& _meshNameLow);

	bool GetIsActive(void) { return isActive; }
	Mesh* GetCurrMesh(void) const;
	Mesh* GetMesh(const CLevelOfDetail::LEVEL _level);
	CLevelOfDetail::LEVEL GetLevel(void) { return level; }

	void SetIsActive(bool _isActive) { isActive = _isActive; }
	bool SetLevel(const CLevelOfDetail::LEVEL _level);

	void DestroyLOD(void);
	

	Mesh* high;
	Mesh* medium;
	Mesh* low;
protected:
	bool isActive;
	LEVEL level;
};

#endif