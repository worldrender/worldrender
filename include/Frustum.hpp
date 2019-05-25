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
    void NormalizePlane(glm::vec4 &frustum_plane);
    void SettingFrustum(glm::mat4 proj_matrix, glm::mat4 view_matrix);

  public:
    // Constructor
    Frustum(glm::mat4 proj_matrix, glm::mat4 view_matrix);

    // Instance methods
    bool  ContainsPoint(const glm::vec3 &point) const;
    int   ContainsSphere(const glm::vec3 &position, const double radius) const;
    void Update(glm::mat4 proj_matrix, glm::mat4 view_matrix);
};

#endif
