#include "bvh.h"
#include "model.h"
#include "triangle.h"

#include <algorithm>
#include <iostream>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min{min}, max{max}{}

bool AABB::intersect(const Ray& ray)
{
	const float t1 = (min.x - ray.origin_.x) / ray.direction_.x;
	const float t2 = (max.x - ray.origin_.x) / ray.direction_.x;
	const float t3 = (min.y - ray.origin_.y) / ray.direction_.y;
	const float t4 = (max.y - ray.origin_.y) / ray.direction_.y;
	const float t5 = (min.z - ray.origin_.z) / ray.direction_.z;
	const float t6 = (max.z - ray.origin_.z) / ray.direction_.z;

	const float tmin = MAX(MAX(MIN(t1, t2), MIN(t3, t4)), MIN(t5, t6));
	const float tmax = MIN(MIN(MAX(t1, t2), MAX(t3, t4)), MAX(t5, t6));

	return tmax > 0.0f && tmin < tmax;
}

static bool tcompare(const Triangle* t0, const Triangle* t1)
{
	return t0->getCenter().x < t1->getCenter().x;
}

BVH::BVH(Mesh* mesh)
{
	this->mesh = mesh;
	auto begin = this->mesh->faces.begin();
	auto end = this->mesh->faces.end();
	glm::vec3 min, max;

	min = mesh->faces[0]->getCenter();
	max = mesh->faces[0]->getCenter();

	for(auto t : mesh->faces)
	{
		min.y = MIN(t->v0.y, min.y);
		min.y = MIN(t->v1.y, min.y);
		min.y = MIN(t->v2.y, min.y);
		max.y = MAX(t->v0.y, max.y);
		max.y = MAX(t->v1.y, max.y);
		max.y = MAX(t->v2.y, max.y);

		min.z = MIN(t->v0.z, min.z);
		min.z = MIN(t->v1.z, min.z);
		min.z = MIN(t->v2.z, min.z);
		max.z = MAX(t->v0.z, max.z);
		max.z = MAX(t->v1.z, max.z);
		max.z = MAX(t->v2.z, max.z);
	}

	std::sort(begin, end, tcompare);
	root = build(5, 0, mesh->faces.size());
}

BVHNode* BVH::build(int length, int begin, int end)
{
	glm::vec3 min, max;
	min = max = mesh->faces[begin]->getCenter();
	for(int i = begin; i < end; i++)
	{
		auto t = mesh->faces[i];
		min.x = MIN(t->v0.x, min.x);
		min.x = MIN(t->v1.x, min.x);
		min.x = MIN(t->v2.x, min.x);
		max.x = MAX(t->v0.x, max.x);
		max.x = MAX(t->v1.x, max.x);
		max.x = MAX(t->v2.x, max.x);

		min.y = MIN(t->v0.y, min.y);
		min.y = MIN(t->v1.y, min.y);
		min.y = MIN(t->v2.y, min.y);
		max.y = MAX(t->v0.y, max.y);
		max.y = MAX(t->v1.y, max.y);
		max.y = MAX(t->v2.y, max.y);

		min.z = MIN(t->v0.z, min.z);
		min.z = MIN(t->v1.z, min.z);
		min.z = MIN(t->v2.z, min.z);
		max.z = MAX(t->v0.z, max.z);
		max.z = MAX(t->v1.z, max.z);
		max.z = MAX(t->v2.z, max.z);
	}

	AABB box(min, max);
	BVHNode *node, *right = NULL, *left = NULL;
	if(length > 0)
	{
		int mid = (begin + end) / 2;
		right = build(length - 1, begin, mid);
		left = build(length - 1, mid, end);
	}
	node = new BVHNode{right, left, begin, end, box};
	
	return node;
}

static bool recursiveIntersect(const Ray& ray, IntersectionRecord& ir, BVHNode *node, const Mesh* mesh)
{
	IntersectionRecord pr;
	bool result = false;

	if(node->left == NULL)
	{
		for(int i = node->begin; i < node->end; i++)
		{
			if(mesh->faces[i]->intersect(ray, pr))
			if(pr.t_ < ir.t_ && pr.t_ > 0.0f)
			{
				ir = pr;
				result = true;
			}
		}
	}
	else
	{
		bool a = false, b = false;
		if(node->left->box.intersect(ray))
			a = recursiveIntersect(ray, ir, node->left, mesh);
		if(node->right->box.intersect(ray))
			b = recursiveIntersect(ray, ir, node->right, mesh);
		result = a || b;
	}

	return result;
}

bool BVH::intersect(const Ray& ray, IntersectionRecord& ir)
{
	return recursiveIntersect(ray, ir, root, mesh);
}