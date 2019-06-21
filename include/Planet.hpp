#ifndef PLANET_HPP_
#define PLANET_HPP_

#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "QuadTree.hpp"

struct Cube
{
  std::shared_ptr<QuadTree> Front = nullptr;
  std::shared_ptr<QuadTree> Back = nullptr;
  std::shared_ptr<QuadTree> Top = nullptr;
  std::shared_ptr<QuadTree> Bottom = nullptr;
  std::shared_ptr<QuadTree> Left = nullptr;
  std::shared_ptr<QuadTree> Right = nullptr;
};

class Planet
{
  private:
    GLfloat radius;
    Cube* cube;
  public:
    static GLuint shader;
    static GLuint VAI;
    static GLuint VAO;
    static GLuint VBO;
    static GLuint VNI;
    Planet();
    Planet(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
       glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 v7,
       GLfloat radius);
    Planet(Cube* cube, GLfloat radius);
    GLfloat getRadius();
    Cube* getCube(){return this->cube;}
};

#endif
