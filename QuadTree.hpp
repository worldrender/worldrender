#ifndef QUADTREE_H_
#define QUADTREE_H_

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <memory>

class QuadTree;

struct Neighbor
{
  Neighbor(QuadTree* first, QuadTree* second):first(first),second(second){}
  QuadTree* first;
  QuadTree* second;
};

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
    std::unique_ptr<Quad> quad   = nullptr;
    std::unique_ptr<QuadTree> nw = nullptr;
    std::unique_ptr<QuadTree> sw = nullptr;
    std::unique_ptr<QuadTree> ne = nullptr;
    std::unique_ptr<QuadTree> se = nullptr;

    QuadTree* parent = nullptr;

    std::unique_ptr<Neighbor> North = nullptr;
    std::unique_ptr<Neighbor> South = nullptr;
    std::unique_ptr<Neighbor> East  = nullptr;
    std::unique_ptr<Neighbor> West  = nullptr;

    GLuint index;
  public:
    static std::vector<glm::vec3> vertices;
    static std::vector<glm::vec3> normals;
    static std::vector<GLushort> indices;
    static std::vector<GLushort> normalIndices;
    static std::vector<GLfloat> noises;
    static std::vector<QuadTree*> quadTreeList;

    QuadTree();
    QuadTree(std::unique_ptr<Quad> quad);

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

