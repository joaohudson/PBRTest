#include "perspective_camera.h"

PerspectiveCamera::PerspectiveCamera( void ){}
PerspectiveCamera::PerspectiveCamera(
	float angle,
	const glm::ivec2 &resolution,
	const glm::vec3 &position,
	const glm::vec3 &up_vector,
	const glm::vec3 &look_at ) :
	Camera(resolution, position, up_vector, look_at)
	{
		this->width = std::tan(angle);
		this->height = std::tan(angle);
	}
					   
Ray PerspectiveCamera::getWorldSpaceRay( const glm::vec2 &pixel_coord ) const
{
	float minX = width * -0.5f;
	float minY = height * -0.5f;

    glm::vec3 origin{ pixel_coord[0] / static_cast< float >( resolution_[0] ) * width + minX,
                      pixel_coord[1] / static_cast< float >( resolution_[1] ) * height + minY,
                      0.0f };

	origin = onb_.getBasisMatrix() * origin;	

    return Ray{ position_, 
                glm::normalize( origin + direction_ ) };
}