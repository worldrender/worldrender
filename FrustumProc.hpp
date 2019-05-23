/*******************************************************************************
* File:           Frustum.h
* Author:         Christian Alfons
* Date created:   2010-07-08
* Date modified:  2010-09-20
* Description:    A simple class for efficient OpenGL view frustum culling.
*******************************************************************************/

#ifndef FRUSTUM_H_INCLUDED
#define FRUSTUM_H_INCLUDED

#include <vector>
#include <glm/glm.hpp>

#define OUTSIDE       0
#define INSIDE        1
#define INTERSECTING  2

using std::vector;

class Frustum
{
  private:
    // Instance variables
    glm::vec4<double> frustum_planes[6];
    double CalculateDistanceToPlane(const int plane, const glm::vec3<double> &point) const;
    void NormalizePlanes();

  public:
    // Constructor
    Frustum();

    // Instance methods
    bool  ContainsPoint(const glm::vec3<double> &point) const;
    int   ContainsSphere(const glm::vec3<double> &position, const double radius) const;
    int   ContainsBox(const glm::vec3<double> &min, const glm::vec3<double> &max) const;
    int   ContainsPolygon(const vector<glm::vec3<double> > &points) const;
};

#endif
