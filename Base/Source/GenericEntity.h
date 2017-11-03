#ifndef GENERIC_ENTITY_H
#define GENERIC_ENTITY_H

#include "EntityBase.h"
#include <string>
#include "Collider/Collider.h"

class Mesh;
/*For categorising entity.*/
enum class ECEntityTypes;
class GenericEntity : public EntityBase, public CCollider
{
public:
	GenericEntity(Mesh* _modelMesh);
	virtual ~GenericEntity();

	virtual void Update(double _dt);
	virtual void Render();

	// Set the maxAABB and minAABB
	void SetAABB(Vector3 maxAABB, Vector3 minAABB);
private:
	Mesh* modelMesh;
	/*Demo-ing Spatial Partitioning.*/
	float timer;
	bool translateDirection;
};

namespace Create
{
	GenericEntity* Entity(	const std::string& _meshName, 
							const Vector3& _position, 
							const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f));
};

#endif // GENERIC_ENTITY_H