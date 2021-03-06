#ifndef MASTERENTITYMANAGER_H
#define MASTERENTITYMANAGER_H

#include <vector>
#include <list>
#include <map>
#include "../EntityTypes/EntityTypes.h"

class EntityBase;
class EntityManager;
class CMasterEntityManager
{
	static CMasterEntityManager* s_instance;
public:
	CMasterEntityManager(void);
	~CMasterEntityManager(void);
	static CMasterEntityManager* GetInstance(void);

	void Init(size_t numOfEntityManager);
	void Update(double _dt);
	void Render();
	void RenderUI();
	void RenderStatic(void);
	void RenderStaticUI(void);

	void AddEntity(EntityBase* _newEntity);
	void RemoveEntity(EntityBase* _newEntity);
	void AddStaticEntity(EntityBase* _newEntity);

	/*Setter*/
	void SetPartitionNum(unsigned _partitionNum) { partitionNum = _partitionNum; }
	/*Turn Spatial Partitioning on and off.*/
	void SetToggle(void) { spatialPartitioning = spatialPartitioning ? false : true; }

	/*Getter*/
	unsigned GetPartitionNum(void) { return partitionNum; }
	bool GetToggle(void) { return spatialPartitioning; }
private:
	typedef std::vector<EntityManager*>MasterList;
	MasterList masterList;
	unsigned partitionNum;

	EntityManager* staticList;

	/*Toggle Spatial Partitioning.*/
	bool spatialPartitioning;

	/*Check if objects belong in the correct partition base on position*/
	void CheckPartition(EntityBase * _entity);

	typedef std::list<EntityBase*> EntityList;
	EntityList allEntities[(unsigned int)(ECEntityTypes::TYPE_COUNT)];
protected:
};

#endif