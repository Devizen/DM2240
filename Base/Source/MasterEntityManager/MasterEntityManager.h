#ifndef MASTERENTITYMANAGER_H
#define MASTERENTITYMANAGER_H

#include <vector>
#include <map>

class CPartitionInfo;
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
	void RemoveEntity(EntityBase* _newEntity, CPartitionInfo* _partitionInfo);
	void AddStaticEntity(EntityBase* _newEntity);

	/*Setter*/
	void SetPartitionNum(unsigned _partitionNum) { partitionNum = _partitionNum; }
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
	void CheckPartition(EntityBase * _entity, CPartitionInfo* _partitionInfo);
	/*Im thinking we need a master list of entities so we can easily remove our entity from its manager*/
	//Should i use map? is it faster? im thinking its slower but im not sure
	std::map<EntityBase*, EntityManager*> allEntities;
protected:
};

#endif