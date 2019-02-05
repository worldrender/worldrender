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
  public:
    Planet();
};

#endif

