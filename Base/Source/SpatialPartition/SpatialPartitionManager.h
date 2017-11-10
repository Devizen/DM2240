#ifndef SPATIALPARTITIONMANAGER_H
#define SPATIALPARTITIONMANAGER_H

/*Pair*/
#include <utility>
#include <string>
#include <vector>
#include "Vector3.h"
class CSpatialPartition;
class CPlayerInfo;
class EntityBase;
class CSpatialPartitionManager
{
	static CSpatialPartitionManager* s_instance;
public:
	CSpatialPartitionManager(void);
	~CSpatialPartitionManager(void);
	static CSpatialPartitionManager* GetInstance(void);
	/*Initialise square grid system by using row (numOfPartition) * column (numofPartition) and size of the ground.*/
	void Init(unsigned _numOfPartition, unsigned _sizeOfSpace, CPlayerInfo* _player);
	
	/*Get player object for calculation of distance.*/
	CPlayerInfo* GetPlayer(void) { return player; }

	/*Check and assign appropriate grid information according to the position that was passed in.*/
	CSpatialPartition* UpdateGridInfo(Vector3 _position);
	/*Get the CSpatialPartition object according to index.*/
	CSpatialPartition* GetPartition(unsigned partitionNum) { return partitionList[partitionNum]; }
	/*Get the total amount of partition(s).*/
	size_t GetPartitionCount(void) { return partitionList.size(); }
	bool IsEntityInCorrectGrid(EntityBase* entity);

	/*Get Grid Length.*/
	float GetGridLength(void) { return gridSize; }
private:
	typedef std::vector<CSpatialPartition*> Partition;
	/*A vector of CSpatialPartition objects.*/
	Partition partitionList;
	/*Player object.*/
	CPlayerInfo* player;
	/*Grid size for length and width.*/
	float gridSize;
protected:
};

#endif