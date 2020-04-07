#ifndef PERSPECTIVE_CAMERA
#define PERSPECTIVE_CAMERA

#include "camera.h"
#include "ray.h"

class PerspectiveCamera : public Camera
{
	public:

	PerspectiveCamera( void );

	PerspectiveCamera( 
					   float angle,
					   const glm::ivec2 &resolution,
                       const glm::vec3 &position,
                       const glm::vec3 &up_vector,
                       const glm::vec3 &look_at );

	Ray getWorldSpaceRay( const glm::vec2 &pixel_coord ) const;

	private:
	float width, height;

};

#endif