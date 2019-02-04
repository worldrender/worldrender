#include "quads.hpp"
#include "simplex.h"

/** This class is responsible for create quadtree struct and their elements.
 *
 *  This class does the creation of each quad and junction of them to create the quadtrees.
 *  There are three methods more the constructor and destructor, and they are:
 *  Split;
 *  Triangulator;
 *  VerticalSplit;
 *
 *  The first parameter is Quadtree::vertexes, and it is responsible to save all vertexes of each quad.
 *  The second parameter is Quatree::indexes and it is saving three index to create a triangle.
 *  The last is a list to save each quad.
 * \param
 * \return
 *
 */
std::vector<glm::vec3> Quadtree::vertices;
std::vector<GLfloat> Quadtree::noises;
std::vector<GLushort> Quadtree::indices;
std::vector<Quadtree*> Quadtree::quadTreeList;

/**
 * Default constructor for the \class Quadtree class, it instantiates a quadtree without any children or quad coordinates
 */

Quadtree::Quadtree():
    nw{nullptr},
    ne{nullptr},
    se{nullptr},
    sw{nullptr},
    quad{}
{
}

/**
 * Constructor for the \class Quadtree class,
 * @param a \struct Quad
 *  This constructor receives a parameter called quad, and it is being a Quad type, and it defines a Quadtree.
 *
 */

Quadtree::Quadtree(Quad *quad):
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

Quadtree::~Quadtree()
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

void Quadtree::split()
{



  if(this->quad->split)
    return;


  glm::vec3 North  = {(Quadtree::vertices[this->quad->c0]+Quadtree::vertices[this->quad->c1])/2.f}, //nIndex
            East   = {(Quadtree::vertices[this->quad->c1]+Quadtree::vertices[this->quad->c2])/2.f}, //Quadtree::vertices.size()-4
            South  = {(Quadtree::vertices[this->quad->c2]+Quadtree::vertices[this->quad->c3])/2.f}, //Quadtree::vertices.size()-3
            West   = {(Quadtree::vertices[this->quad->c3]+Quadtree::vertices[this->quad->c0])/2.f}, //wIndex
            Center = {(Quadtree::vertices[this->quad->c0]+Quadtree::vertices[this->quad->c2])/2.f}; //Quadtree::vertices.size()-1
  GLuint i = 0,
  nIndex = -1,
  eIndex = -1,
  sIndex = -1,
  wIndex = -1,
  cIndex = -1;
  for(auto &vertex : Quadtree::vertices)
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
    Quadtree::vertices.push_back(North);
    nIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(eIndex==-1u)
  {
    Quadtree::vertices.push_back(East);
    eIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(sIndex==-1u)
  {
    Quadtree::vertices.push_back(South);
    sIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(wIndex==-1u)
  {
    Quadtree::vertices.push_back(West);
    wIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(cIndex==-1u)
  {
    Quadtree::vertices.push_back(Center);
    cIndex = (int)Quadtree::vertices.size() - 1;
  }

  Quad nw = {this->quad->c0,nIndex,cIndex,wIndex},
       ne = {nIndex,this->quad->c1,eIndex,cIndex},
       se = {cIndex,eIndex,this->quad->c2,sIndex},
       sw = {wIndex,cIndex,sIndex,this->quad->c3};

  this->nw = new Quadtree(&nw);
  this->ne = new Quadtree(&ne);
  this->sw = new Quadtree(&sw);
  this->se = new Quadtree(&se);

  this->quad->split = true;
}

/**
 * This method creates two triangles for each quad. If the quad is not split,  it returns.
 */

void Quadtree::triangulator(){
  indices.clear();
  for(auto &quad : Quadtree::quadTreeList)
  {
    if(!quad->split)
    {
      indices.push_back(quad->c0);
      indices.push_back(quad->c1);
      indices.push_back(quad->c2);

      indices.push_back(quad->c1);
      indices.push_back(quad->c3);
      indices.push_back(quad->c2);
    }
  }
}
/**
 * This method splits for each quad on quadtree list.
 */

void Quadtree::verticalSplit(GLuint lod){
  for(int i=0;i<lod;i++)
  {
    for(auto &quad : Quadtree::quadTreeList)
    {
      quad->split();
    }
  }
}

void Quadtree::instanceNoise(){
  for(auto &vertex : Quadtree::vertices)
  {
    noises.push_back(Simplex::iqfBm(vertex));
  }
}
