#include "LevelOfDetail.h"
#include "MeshBuilder.h"

CLevelOfDetail::CLevelOfDetail()
	: high(nullptr)
	, medium(nullptr)
	, low(nullptr)
	, isActive(false)
	, level(NONE)
{
}

CLevelOfDetail::~CLevelOfDetail(void)
{
	if (high != nullptr) {
		delete high;
		high = nullptr;
	}
	if (medium != nullptr) {
		delete medium;
		medium = nullptr;
	}
	if (low != nullptr) {
		delete low;
		low = nullptr;
	}
}

bool CLevelOfDetail::Init(const std::string & _meshNameHigh, const std::string & _meshNameMedium, const std::string & _meshNameLow)
{
	high = MeshBuilder::GetInstance()->GetMesh(_meshNameHigh);
	medium = MeshBuilder::GetInstance()->GetMesh(_meshNameMedium);
	low = MeshBuilder::GetInstance()->GetMesh(_meshNameLow);

	if (high == nullptr ||
		medium == nullptr ||
		low == nullptr)
		return false;

	SetIsActive(true);
	return true;
}

Mesh * CLevelOfDetail::GetCurrMesh(void) const
{
	if (level == HIGH)
		return high;
	else if (level == MEDIUM)
		return medium;
	else if (level == LOW)
		return low;

	return nullptr;
}

Mesh * CLevelOfDetail::GetMesh(const CLevelOfDetail::LEVEL _level)
{
	if (_level == HIGH)
		return high;
	else if (_level == MEDIUM)
		return medium;
	else if (_level == LOW)
		return low;

	return nullptr;
}

bool CLevelOfDetail::SetLevel(const CLevelOfDetail::LEVEL _level)
{
	if (_level > CLevelOfDetail::NONE && _level < CLevelOfDetail::TOTAL) {
		level = _level;
		return true;
	}
	return false;
}

void CLevelOfDetail::DestroyLOD(void)
{
	if (high)
		delete high;
	if (medium)
		delete medium;
	if (low)
		delete low;
}
