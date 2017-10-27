#ifndef SPATIALPARTITIONMANAGER_H
#define SPATIALPARTITIONMANAGER_H

/*Pair*/
#include <utility>
#include <string>
#include <vector>
#include "Vector3.h"
class CSpatialPartition;
class CPlayerInfo;
class CPartitionInfo;
class CSpatialPartitionManager
{
	static CSpatialPartitionManager* s_instance;
public:
	CSpatialPartitionManager(void);
	~CSpatialPartitionManager(void);
	static CSpatialPartitionManager* GetInstance(void);
	void Init(unsigned numOfPartition, unsigned sizeOfSpace, CPlayerInfo* _player);
	unsigned GetPlayerGrid(void);
	unsigned GetEntityGrid(Vector3 _position);
	CSpatialPartition* GetPartition(unsigned partitionNum);

	size_t GetPartitionCount(void) { return partitionList.size(); }
	bool IsEntityInCorrectGrid(CPartitionInfo* partitionInfo);
private:
	typedef std::pair<std::string, CSpatialPartition*> Partition;
	std::vector<Partition>partitionList;
	CPlayerInfo* player;
protected:
};

#endif