#ifndef SPATIALPARTITIONMANAGER_H
#define SPATIALPARTITIONMANAGER_H

/*Pair*/
#include <utility>
#include <string>
#include <vector>
class CSpatialPartition;
class CPlayerInfo;
class CSpatialPartitionManager
{
	static CSpatialPartitionManager* s_instance;
public:
	CSpatialPartitionManager(void);
	~CSpatialPartitionManager(void);
	static CSpatialPartitionManager* GetInstance(void);
	void Init(unsigned numOfPartition, unsigned sizeOfSpace, CPlayerInfo* _player);
	unsigned GetPlayerGrid(void);
private:
	typedef std::pair<std::string, CSpatialPartition*> Partition;
	std::vector<Partition>partitionList;
	CPlayerInfo* player;
protected:
};

#endif