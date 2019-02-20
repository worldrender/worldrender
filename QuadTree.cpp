#include "QuadTree.hpp"
#include "simplex.h"
#include <iostream>

using namespace std;

/** This class is responsible for create quadtree struct and their elements.
 *
 *  This class does the creation of each quad and junction of them to create the quadtrees.
 *  There are three methods more the constructor and destructor, and they are:
 *  Split;
 *  Triangulator;
 *  VerticalSplit;
 *
 *  The first parameter is QuadTree::vertexes, and it is responsible to save all vertexes of each quad.
 *  The second parameter is Quatree::indexes and it is saving three index to create a triangle.
 *  The last is a list to save each quad.
 * \param
 * \return
 *
 */

std::vector<glm::vec3> QuadTree::vertices;
std::vector<glm::vec3> QuadTree::normals;
std::vector<GLfloat> QuadTree::noises;
std::vector<GLushort> QuadTree::indices;
std::vector<GLushort> QuadTree::normalIndices;
std::vector<QuadTree*> QuadTree::quadTreeList;

/**
 * Default constructor for the \class QuadTree class, it instantiates a quadtree without any children or quad coordinates
 */

QuadTree::QuadTree():
    nw{nullptr},
    ne{nullptr},
    se{nullptr},
    sw{nullptr},
    quad{}
{
}

/**
 * Constructor for the \class QuadTree class,
 * @param a \struct Quad
 *  This constructor receives a parameter called quad, and it is being a Quad type, and it defines a QuadTree.
 *
 */

QuadTree::QuadTree(Quad *quad):
    nw{nullptr},
    ne{nullptr},
    sw{nullptr},
    se{nullptr}
{
  this->quad = quad;
  this->index = this->quadTreeList.size();
  this->quadTreeList.push_back(this);
}

/**
 *  This method is responsible to delete each vertex of this quad;
 */

QuadTree::~QuadTree()
{
    delete nw;
    delete ne;
    delete sw;
    delete se;
}

/**
 * This method is responsible to split a quad to create four new quads on quadtree.
 *
 * If that quad is splitted, that method finish it. If not, it is calculated for each side of quad to calculate each vertex.
 * When it will finish, each calculated vertex is added on vertex list and is created four new quads and it will mark the parameter "quad->split" true;
 */

void QuadTree::split()
{
  using namespace glm;
  if(this->quad->split)
    return;
  vec3 North  = (vertices[this->quad->c0]+vertices[this->quad->c1])/2.f;
  vec3 East  = (vertices[this->quad->c1]+vertices[this->quad->c2])/2.f;
  vec3 South  = (vertices[this->quad->c2]+vertices[this->quad->c3])/2.f;
  vec3 West   = (vertices[this->quad->c3]+vertices[this->quad->c0])/2.f;
  vec3 Center = (vertices[this->quad->c0]+vertices[this->quad->c2])/2.f;

  GLuint i = 0,
  nIndex = -1,
  eIndex = -1,
  sIndex = -1,
  wIndex = -1,
  cIndex = -1;
  for(auto &vertex : QuadTree::vertices)
  {
    if(vertex == North)
      nIndex = i;
    if(vertex == East)
      eIndex = i;
    if(vertex == South)
      sIndex = i;
    if(vertex == West)
      wIndex = i;
    if(vertex == Center)
      cIndex = i;
    if(nIndex>-1u&&eIndex>-1u&&sIndex>-1u&&wIndex>-1u&&cIndex>-1u)
      break;
    i++;
  }
  if(nIndex==-1u)
  {
    QuadTree::vertices.push_back(North);
    nIndex = (GLuint)QuadTree::vertices.size() - 1;
  }
  if(eIndex==-1u)
  {
    QuadTree::vertices.push_back(East);
    eIndex = (GLuint)QuadTree::vertices.size() - 1;
  }
  if(sIndex==-1u)
  {
    QuadTree::vertices.push_back(South);
    sIndex = (GLuint)QuadTree::vertices.size() - 1;
  }
  if(wIndex==-1u)
  {
    QuadTree::vertices.push_back(West);
    wIndex = (GLuint)QuadTree::vertices.size() - 1;
  }
  if(cIndex==-1u)
  {
    QuadTree::vertices.push_back(Center);
    cIndex = (GLuint)QuadTree::vertices.size() - 1;
  }

  Quad nw = Quad(this->quad->c0,nIndex,cIndex,wIndex),
       ne = Quad(nIndex,this->quad->c1,eIndex,cIndex),
       se = Quad(cIndex,eIndex,this->quad->c2,sIndex),
       sw = Quad(wIndex,cIndex,sIndex,this->quad->c3);

  this->nw = new QuadTree(&nw);
  this->ne = new QuadTree(&ne);
  this->sw = new QuadTree(&sw);
  this->se = new QuadTree(&se);

  this->quad->split = true;
}

/**
 * This method creates two triangles for each quad. If the quad is not split,  it returns.
 */

void QuadTree::triangulator(){
  indices.clear();
  for(auto &quad : QuadTree::quadTreeList)
  {
    if(!quad->quad->split)
    {
      indices.push_back(quad->quad->c0);
      indices.push_back(quad->quad->c1);
      indices.push_back(quad->quad->c2);

      indices.push_back(quad->quad->c2);
      indices.push_back(quad->quad->c3);
      indices.push_back(quad->quad->c0);
    }
  }
}
/**
 * This method splits for each quad on quadtree list.
 */

void QuadTree::verticalSplit(GLuint lod){
  for(GLuint i=0;i<lod;i++)
  {
    for(auto &quad : QuadTree::quadTreeList)
    {
      quad->split();
    }
  }
}

void QuadTree::instanceNoise(){
  for(auto &vertex : QuadTree::vertices)
  {
    noises.push_back(Simplex::iqfBm(vertex));
  }
}
