#include "SpatialPartitionManager.h"
#include "SpatialPartition.h"
#include "../PlayerInfo/PlayerInfo.h"
#include <iostream>
#include "EntityBase.h"

CSpatialPartitionManager* CSpatialPartitionManager::s_instance = 0;

CSpatialPartitionManager::CSpatialPartitionManager(void) : toggle(true)
{
}

CSpatialPartitionManager::~CSpatialPartitionManager(void)
{
}

CSpatialPartitionManager * CSpatialPartitionManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CSpatialPartitionManager();

	return s_instance;
}

void CSpatialPartitionManager::Init(unsigned _numOfPartition, unsigned _sizeOfSpace, CPlayerInfo* _player)
{
	std::cout << _numOfPartition << " : " << _sizeOfSpace << std::endl;
	player = _player;
	const float minY = -10.f;
	const float maxY = 10.f;
	const float gridSize = static_cast<float>(_sizeOfSpace) / static_cast<float>(_numOfPartition);
	this->gridSize = gridSize;
	Vector3 startPoint(-static_cast<float>(_sizeOfSpace / 2), -10.f, -static_cast<float>(_sizeOfSpace / 2));
	Vector3 _minBoundary(startPoint);
	Vector3 _maxBoundary(startPoint.x + gridSize, startPoint.y + _sizeOfSpace, startPoint.z + gridSize);
	Vector3 startPosition((_minBoundary.x + _maxBoundary.x) * 0.5f, _minBoundary.y, (_minBoundary.z + _maxBoundary.z) * 0.5f);
	Vector3 _position(startPosition);

	for (unsigned column = 0; column < _numOfPartition; ++column)
	{
		for (unsigned row = 0; row < _numOfPartition; ++row)
		{
			CSpatialPartition* partition = new CSpatialPartition((column * _numOfPartition) + row, _position, _minBoundary, _maxBoundary);

			//std::string indexOfPartition = std::to_string((column * numOfPartition) + row);
			//partitionList.push_back(Partition(indexOfPartition, partition));

			partitionList.push_back(partition);

			_position = Vector3(_position.x + gridSize, _position.y, _position.z);
			_minBoundary = Vector3(_minBoundary.x + gridSize, _minBoundary.y, _minBoundary.z);
			_maxBoundary = Vector3(_maxBoundary.x + gridSize, _minBoundary.y + _sizeOfSpace, _maxBoundary.z);
		}
		_position.x = startPosition.x;
		_position.z += gridSize;
		_minBoundary = Vector3(startPoint.x, startPoint.y, _minBoundary.z + gridSize);
		_maxBoundary = Vector3(_minBoundary.x + gridSize, _position.y + _sizeOfSpace, _minBoundary.z + gridSize);
	}

	std::cout << "Size of List: " << partitionList.size() << std::endl;
	//-100, -100 : 100, 100
	//When map starts,
	//Left top   : Right bottom
	/*
	200 Long
	|--------------------|
	Create 10 Grids
	Starts at -100, -100
	200 / 10 = 20
	1. -100, -100
	2. -80, -100
	3. -60, -100
	4. -40, -100
	5. -20, -100
	6. 0, -100
	7. 20, -100
	8. 40, -100
	9. 60, -100
	10. 80, -100

	1. minBoundary = Vector3(-100.f, -10.f, -100.f);
	   maxBoundary = Vector3(-80.f, 10.f, -80.f);
	2. minBoundary = Vector3(-80.f, -10.f, -100.f);
	   maxBoundary = Vector3(-60.f, 10.f, -80.f);
	*/
}

CSpatialPartition* CSpatialPartitionManager::UpdateGridInfo(Vector3 _position)
{
	for (size_t i = 0; i < partitionList.size(); ++i)
	{
		if (_position >= partitionList[i]->GetMinBoundary() && _position <= partitionList[i]->GetMaxBoundary())
			return partitionList[i];
	}
	return nullptr;
}

//unsigned CSpatialPartitionManager::GetEntityGrid(Vector3 _position)
//{
//	unsigned grid = 0;
//
//	for (std::vector<Partition>::iterator it = partitionList.begin(); it != partitionList.end(); ++it)
//	{
//		if (_position >= it->second->GetMinBoundary() &&
//			_position <= it->second->GetMaxBoundary())
//		{
//			grid = std::stoi(it->first);
//			std::cout << "CHECKING" << std::endl;
//			break;
//		}
//	}
//	return grid;
//}

//CSpatialPartition * CSpatialPartitionManager::GetPartition(unsigned partitionNum)
//{
//	return partitionList[partitionNum].second;
//}

bool CSpatialPartitionManager::IsEntityInCorrectGrid(EntityBase * entity)
{
	CSpatialPartition* myPartition = dynamic_cast<CSpatialPartition*>(entity);
	return (entity->GetPosition() >= myPartition->GetMinBoundary() &&
		entity->GetPosition() <= myPartition->GetMaxBoundary());
	return false;
}

std::list<unsigned> CSpatialPartitionManager::GetPartitionIndices(Vector3 position, Vector3 scale)
{
	std::list<unsigned> index;
	Vector3 min = position - scale;
	Vector3 max = position + scale;
	for (size_t i = 0; i < partitionList.size(); ++i)
	{
		if (max >= partitionList[i]->GetMinBoundary() && min <= partitionList[i]->GetMaxBoundary())
		{
			if (std::find(index.begin(), index.end(), i) == index.end())
				index.push_back(i);
		}
	}

	return index;
}
