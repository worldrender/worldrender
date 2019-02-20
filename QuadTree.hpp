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
  Quad(GLuint c0, GLuint c1, GLuint c2, GLuint c3)
  {
    this->c0 = c0;
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;
  }
  Quad(){}
};

class QuadTree
{
  private:
    Quad* quad   = nullptr;
    QuadTree* nw = nullptr;
    QuadTree* sw = nullptr;
    QuadTree* ne = nullptr;
    QuadTree* se = nullptr;
    GLuint index;
  public:
    static std::vector<glm::vec3> vertices;
    static std::vector<glm::vec3> normals;
    static std::vector<GLushort> indices;
    static std::vector<GLushort> normalIndices;
    static std::vector<GLfloat> noises;
    static std::vector<QuadTree*> quadTreeList;
    QuadTree();
    QuadTree(Quad *quad);

    ~QuadTree();
    void split();
    static void triangulator();
    static void verticalSplit(GLuint lod);
    static void instanceNoise();
};
#endif

