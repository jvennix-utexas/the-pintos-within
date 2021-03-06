#ifndef __SphereRigidbody_h_
#define __SphereRigidbody_h_

#include "Rigidbody.h"

class SphereRigidbody : Rigidbody
{
public:
	SphereRigidbody(GameObject*, float, float, 
					int, int, btRigidBody::btRigidBodyConstructionInfo* _rigid_info = NULL,
					float x_offset = 0, float y_offset = 0, float z_offset = 0);
	virtual ~SphereRigidbody();

	virtual void update();

	virtual bool needsCollision(btBroadphaseProxy*) const;

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		   	const btCollisionObject*, int, int,
		    const btCollisionObject*, int, int);

};

#endif // #ifndef __SphereRigidbody_h_