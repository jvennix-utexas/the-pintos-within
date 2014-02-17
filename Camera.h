#ifndef __Camera_h_
#define __Camera_h_

#include "common.h"

class Camera : Component
{
	protected:
		Transform* _transform;

	public:
		Camera();
		~Camera();

		Ogre::Viewport* view_port;
		Ogre::Camera* camera;

		virtual void update();
};

#endif // #ifndef __Camera_h_
