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
    Planet(glm::vec3 c0, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3,
           glm::vec3 c4, glm::vec3 c5, glm::vec3 c6, glm::vec3 c7,
           GLfloat radius);
    Planet(Cube* cube, GLfloat radius);
};

#endif

