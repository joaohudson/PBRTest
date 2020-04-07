#ifndef _MATERIAL_
#define _MATERIAL_

#include <glm/glm.hpp>

enum MaterialType{
	DIFFUSE, SPECULAR, LIGHT, DIELECTRIC
};

struct Material
{
	glm::vec3 color;
	glm::vec3 emission;
	MaterialType type;

	Material(){//Make default material is white diffuse
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		emission = glm::vec3();
		type = MaterialType::DIFFUSE;
	}
};


#endif
