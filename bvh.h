#ifndef _BVH_
#define _BVH_

#include <glm/glm.hpp>
#include "ray.h"
#include "intersection_record.h"

class Mesh;

class AABB{
	public:
	AABB(const glm::vec3& min, const glm::vec3& max);
	bool intersect(const Ray& ray);
	private:
	glm::vec3 min, max;
};

struct BVHNode{
	BVHNode *right, *left;
	int begin, end;
	AABB box;
};

class BVH{
	public:
	BVH(Mesh *mesh);
	bool intersect(const Ray& ray, IntersectionRecord& ir);

	private:
	BVHNode* build(int length, int begin, int end);
	Mesh *mesh;
	BVHNode *root;
};

#endif