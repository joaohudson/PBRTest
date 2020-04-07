#ifndef _MODEL_
#define _MODEL_

#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "bvh.h"
#include "material.h"
#include "triangle.h"

struct Mesh{
	std::vector<Triangle*> faces;
	Material material;
	BVH *bvh;

	~Mesh(){ delete bvh; }
};

class Model : public Primitive
{
	public:
	Model(const Material& defaultMaterial, const std::string& filename);
	~Model();
	bool intersect(const Ray& ray, IntersectionRecord& intersection_record) const;
	void addMaterial(const std::string& meshName, const Material& material);

	private:
	void load(const std::string& fileName, const Material& defaultMaterial);
	std::map<std::string, Mesh*> data;
};

#endif
