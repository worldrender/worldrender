#ifndef PLANET_H_
#define PLANET_H_

#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "QuadTree.hpp"

struct Cube
{
  QuadTree* Front = nullptr;
  QuadTree* Back = nullptr;
  QuadTree* Top = nullptr;
  QuadTree* Bottom = nullptr;
  QuadTree* Left = nullptr;
  QuadTree* Right = nullptr;
};

class Planet
{
  private:
    Cube* cube;
    GLfloat radius;
  public:
    Planet();
    Planet(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
       glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 v7,
       GLfloat radius);
    Planet(Cube* cube, GLfloat radius);
    getRadius();
};

#endif
