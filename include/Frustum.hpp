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

class Frustum
{
  public:

    enum Plane
    {
      PLANE_BACK,
      PLANE_FRONT,
      PLANE_RIGHT,
      PLANE_LEFT,
      PLANE_TOP,
      PLANE_BOTTOM
    };

  public:
    Frustum( const mat4 &viewMatrix, const mat4 &projectionMatrix );

    const vec4 &getPlane( const int plane ) const;

    ContainsQuad(std::shared_ptr<QuadTree> quad);

  TestResult testIntersection( const vec3 &point ) const;
  TestResult testIntersection( shared_ptr<const BoundingBox> box ) const;
  TestResult testIntersection( shared_ptr<const BoundingSphere> sphere ) const;

  protected:

  vec4 m_planes[6];
};
#endif
