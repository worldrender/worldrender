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

using std::vector;

class Frustum
{
  private:
    // Instance variables
    glm::vec4 frustum_planes[6];
    double CalculateDistanceToPlane(const int plane, const glm::vec3 &point) const;
    void NormalizePlanes();

  public:
    // Constructor
    Frustum();

    // Instance methods
    int   ContainsSphere(const glm::vec3 &position, const double radius) const;
    void normalizePlane(glm::vec4 &frustum_plane);
    void CalculateFrustum(glm::mat4 &view_matrix, glm::mat4 &proj_matrix);
    bool ContainsPoint(const glm::vec3 &point) const;


};

#endif
