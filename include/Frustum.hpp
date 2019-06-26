#ifndef FRUSTUM_HPP_INCLUDED
#define FRUSTUM_HPP_INCLUDED

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include <vector>

#include "QuadTree.hpp"
#include "Camera.hpp"

#include <GL/gl.h>
#include <iostream>

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

    Frustum( const mat4 &viewMatrix, const mat4 &projectionMatrix );
    ~Frustum() = default;

    const vec4 &getPlane( const int plane ) const;

    void ContainsQuad(std::shared_ptr<QuadTree> quad);

  protected:

  vec4 m_planes[6];


};
#endif
