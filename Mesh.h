#ifndef __Mesh_h_
#define __Mesh_h_

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include "common.h"

#include "Component.h"
#include "Transform.h"

class Mesh : public Component
{
public:
	Transform* _transform;
	
	static int mesh_entity_id_assigner;
	static int mesh_node_id_assigner;
public:
	Mesh(GameObject*, std::string);
	~Mesh();

	Ogre::Entity* entity;
  	Ogre::SceneNode* node;

  	virtual void update();
};
int Mesh::mesh_entity_id_assigner = 0;
int Mesh::mesh_node_id_assigner = 0;

#endif // #ifndef __Mesh_h_