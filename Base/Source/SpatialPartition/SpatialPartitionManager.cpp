#include "SpatialPartitionManager.h"
#include "SpatialPartition.h"
#include "../PlayerInfo/PlayerInfo.h"
#include <iostream>

CSpatialPartitionManager* CSpatialPartitionManager::s_instance = 0;

CSpatialPartitionManager::CSpatialPartitionManager(void)
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

void CSpatialPartitionManager::Init(unsigned numOfPartition, unsigned sizeOfSpace, CPlayerInfo* _player)
{
	std::cout << numOfPartition << " : " << sizeOfSpace << std::endl;
	player = _player;
	const float minY = -10.f;
	const float maxY = 10.f;
	const float gridSize = static_cast<float>(sizeOfSpace) / static_cast<float>(numOfPartition);
	Vector3 startPoint(-static_cast<float>(sizeOfSpace / 2), -10.f, -static_cast<float>(sizeOfSpace / 2));
	Vector3 _minBoundary(startPoint);
	Vector3 _maxBoundary(startPoint.x + gridSize, startPoint.y + gridSize, startPoint.z + gridSize);

	Vector3 _position(startPoint);

	for (unsigned column = 0; column < numOfPartition; ++column)
	{
		for (unsigned row = 0; row < numOfPartition; ++row)
		{
			CSpatialPartition* partition = new CSpatialPartition(_position, _minBoundary, _maxBoundary);
			std::string indexOfPartition = std::to_string((column * numOfPartition) + row);
			partitionList.push_back(Partition(indexOfPartition, partition));
			_position = Vector3(_position.x + gridSize, _position.y, _position.z);
			_minBoundary = _position;
			_maxBoundary = Vector3(_position.x + gridSize, _position.y + sizeOfSpace, _position.z + gridSize);
		}
		_position.x = startPoint.x;
		_position.z += gridSize;
		_minBoundary = _position;
		_maxBoundary = Vector3(_position.x + gridSize, _position.y + sizeOfSpace, _position.z + gridSize);
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

unsigned CSpatialPartitionManager::GetPlayerGrid(void)
{
	unsigned grid = 0;

	for (std::vector<Partition>::iterator it = partitionList.begin(); it != partitionList.end(); ++it)
	{
		if (player->GetPos() >= it->second->GetMinBoundary() &&
			player->GetPos() <= it->second->GetMaxBoundary())
		{
			grid = std::stoi(it->first);
			std::cout << "CHECKING" << std::endl;
			break;
		}
	}
	return grid;
}

unsigned CSpatialPartitionManager::GetEntityGrid(Vector3 _position)
{
	unsigned grid = 0;

	for (std::vector<Partition>::iterator it = partitionList.begin(); it != partitionList.end(); ++it)
	{
		if (_position >= it->second->GetMinBoundary() &&
			_position <= it->second->GetMaxBoundary())
		{
			grid = std::stoi(it->first);
			std::cout << "CHECKING" << std::endl;
			break;
		}
	}
	return grid;
}

CSpatialPartition * CSpatialPartitionManager::GetPartition(unsigned partitionNum)
{
	return partitionList[partitionNum].second;
}
