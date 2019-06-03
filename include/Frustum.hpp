#ifndef FRUSTUM_HPP_INCLUDED
#define FRUSTUM_HPP_INCLUDED

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include <vector>

#include "QuadTree.hpp"
#include "Camera.hpp"

using std::vector;
using namespace glm;

class Camera;

struct Plane
{
	Plane()
	{
		n = vec3(0, 1, 0);
		d = vec3(0, 0, 0);
	}
	Plane(vec3 normal, vec3 det)
	{
		n = normal;
		d = det;
	}
	Plane(vec3 a, vec3 b, vec3 c)
	{
		d = a;
		n = glm::normalize(glm::cross(-b + a, c - a));
	}
	vec3 n;
	vec3 d;
};

struct Sphere
{
	Sphere()
	{
		radius = 1;
		pos = vec3(0, 0, 0);
	}
	Sphere(vec3 position, float size)
	{
		pos = position;
		radius = size;
	}
	vec3 pos;
	float radius;
};

enum class VolumeCheck
{
	OUTSIDE,
	INTERSECT,
	CONTAINS
};

struct FrustumCorners
{
	//Utility to transform frustum into any objects space
	//Useful for complex frustum culling operations
	void Transform(mat4 space);
	//near plane
	vec3 na;
	vec3 nb;
	vec3 nc;
	vec3 nd;
	//far plane
	vec3 fa;
	vec3 fb;
	vec3 fc;
	vec3 fd;
};

class Frustum
{
public:
	Frustum();
	~Frustum();

	void Update();

	void SetToCamera(Camera* pCamera);
	void SetCullTransform(mat4 objectWorld);

	void ContainsQuad(QuadTree *quad);

	const vec3 &GetPositionOS() { return PositionObject; }
	const float GetFOV() { return FOV; }
	const float GetRadInvFOV() { return RadInvFOV; }

	FrustumCorners GetCorners() { return Corners; }

private:
	//transform to the culled objects object space and back to world space
	mat4 CullWorld, CullInverse;

	//stuff in the culled objects object space
	std::vector<Plane> Planes;
	FrustumCorners Corners;
	vec3 PositionObject;

	float RadInvFOV;

	//camera parameters for locking
	vec3 Position;
	vec3 Forward;
	vec3 Up;
	vec3 Right;
	float NearPlane, FarPlane, FOV;
};
#endif
