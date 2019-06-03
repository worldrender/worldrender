#ifndef FRUSTUMPROC_HPP_INCLUDED
#define FRUSTUMPROC_HPP_INCLUDED

#include "QuadTree.hpp"
#include "Frustum.hpp"
#include "Camera.hpp"
#include "Planet.hpp"

using std::vector;
using namespace glm;

class FrustumProc
{
  public:
    FrustumProc(Planet *planet, Camera *camera);
    ~FrustumProc() = default;

    void calculateFrustum();
  private:
    Planet *planet;
    Camera *camera;
    Frustum *frustum;
};
#endif
