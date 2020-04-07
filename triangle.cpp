#include "triangle.h"
#include <iostream>

#define EPSILON 0.00001f

Triangle::Triangle(){}

Triangle::Triangle(
	const glm::vec3& v0,
	const glm::vec3& v1,
	const glm::vec3& v2) : 
	v0{v0}, v1{v1}, v2{v2}
{
	n0 = n1 = n2 = glm::cross(v1 - v0, v2 - v0);
}

Triangle::Triangle(
	const glm::vec3& v0,
	const glm::vec3& v1,
	const glm::vec3& v2,
	const glm::vec3& n0,
	const glm::vec3& n1,
	const glm::vec3& n2) : 
	v0{v0}, v1{v1}, v2{v2}, n0{n0}, n1{n1}, n2{n2}{}

bool Triangle::intersect(const Ray& ray, IntersectionRecord& intersection_record) const
{
	glm::vec3 tvec = ray.origin_ - v0;
	glm::vec3 e0 = v1 - v0;
	glm::vec3 e1 = v2 - v0;
	glm::vec3 pvec = glm::cross(ray.direction_, e1);
	glm::vec3 qvec = glm::cross(tvec, e0);
	float det = (1.0f / glm::dot(pvec, e0));
	float t = glm::dot(qvec, e1) * det;
	float u = glm::dot(pvec,tvec) * det;
	float v = glm::dot(qvec, ray.direction_) * det;
	float w;

	if(t < EPSILON || u < EPSILON  || v < EPSILON  || (u + v > 1.0f))
		return false;

	w = 1.0f - (u + v);

	intersection_record.t_ = t;
	intersection_record.normal_ = glm::normalize(n0 * w + n1 * u + n2 * v);
	intersection_record.position_ = ray.origin_ + ray.direction_ * t;

	return true;
}

glm::vec3 Triangle::getCenter() const
{
	return (v0 + v1 + v2) * 0.3333333333f;
}

void Triangle::show() const
{
	std::cout << "a: x=" << v0.x << "y=" << v0.y << "z=" << v0.z << std::endl;
	std::cout << "b: x=" << v1.x << "y=" << v1.y << "z=" << v1.z << std::endl;
	std::cout << "c: x=" << v2.x << "y=" << v2.y << "z=" << v2.z << std::endl;
}

