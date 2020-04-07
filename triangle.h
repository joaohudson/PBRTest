#ifndef TRIANGLE
#define TRIANGLE

#include "primitive.h"

class Triangle : public Primitive
{
	public:
	Triangle( void );
	
	Triangle(const glm::vec3& v0,
			 const glm::vec3& v1,
			 const glm::vec3& v2);

	Triangle(const glm::vec3& v0,
			 const glm::vec3& v1,
			 const glm::vec3& v2,
			 const glm::vec3& n0,
			 const glm::vec3& n1, 
			 const glm::vec3& n2);
	

	bool intersect(const Ray& ray, IntersectionRecord& intersection_record) const;
	void show(void) const;
	glm::vec3 getCenter() const;

	glm::vec3 v0, v1, v2;

	private:
	glm::vec3 n0, n1, n2;
};


#endif