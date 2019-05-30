#ifndef QUADTREE_H_
#define QUADTREE_H_

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <memory>

#include "Verts.hpp"

class QuadTree;

struct Quad
{
  GLuint c0 = 0u; //northWest
  GLuint c1 = 0u; //northEast
  GLuint c2 = 0u; //southEest
  GLuint c3 = 0u; //southWast
  bool split = false;
  bool isCollapsed = false;
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
    std::unique_ptr<Quad>     quad   = nullptr;
    std::unique_ptr<QuadTree> nw     = nullptr;
    std::unique_ptr<QuadTree> sw     = nullptr;
    std::unique_ptr<QuadTree> ne     = nullptr;
    std::unique_ptr<QuadTree> se     = nullptr;

    QuadTree* parent = nullptr;

    GLuint index;
  public:
    static Verts verts;
    static std::vector<glm::vec3>   normals;
    static std::vector<GLuint>      indices;
    static std::vector<GLuint>      normalIndices;
    static std::vector<GLfloat>     noises;
    static std::vector<QuadTree*>   quadTreeList;
    static std::vector<signed char>        visibility;

    QuadTree();
    QuadTree(std::unique_ptr<Quad> quad, QuadTree* parent);

    ~QuadTree();

    void split();
    void nSplit();

    static void triangulator();
    static void verticalSplit(GLuint lod);
    static void instanceNoise();
    static void instanceNoiseR(int start, int end);
    static void threadedInstanceNoise();
};
#endif

