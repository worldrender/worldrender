#ifndef QUADTREE_H_
#define QUADTREE_H_

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>

struct Quad
{
  GLuint c0 = 0.f; //northWest
  GLuint c1 = 0.f; //northEast
  GLuint c2 = 0.f; //southEest
  GLuint c3 = 0.f; //southWast
  bool split = false;
};

class Quadtree
{
  private:
    Quad quad;
    Quadtree* nw;
    Quadtree* sw;
    Quadtree* ne;
    Quadtree* se;
  public:
    static std::vector<glm::vec3> vertices;
    static std::vector<GLushort> indices;
    Quadtree();
    Quadtree(Quad quad);

    ~Quadtree();
    void split();
};
#endif

