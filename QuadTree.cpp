#include "QuadTree.hpp"
#include "Utils.hpp"
#include "simplex.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


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

QuadTree::QuadTree(std::unique_ptr<Quad> quad):
    nw{nullptr},
    ne{nullptr},
    sw{nullptr},
    se{nullptr}
{
  this->quad = std::move(quad);
  this->index = this->quadTreeList.size();
  this->quadTreeList.push_back(this);
}

/**
 *  This method is responsible to delete each vertex of this quad;
 */

QuadTree::~QuadTree() = default;
/**
 * This method is responsible to split a quad to create four new quads on quadtree.
 *
 * If that quad is splitted, that method finish it. If not, it is calculated for each side of quad to calculate each vertex.
 * When it will finish, each calculated vertex is added on vertex list and is created four new quads and it will mark the parameter "quad->split" true;
 */

void QuadTree::split()
{
  if(this->quad->split)
    return;

  glm::vec3 North  = {(QuadTree::vertices[this->quad->c0] + QuadTree::vertices[this->quad->c1])/2.f}, //nIndex
            East   = {(QuadTree::vertices[this->quad->c1] + QuadTree::vertices[this->quad->c2])/2.f}, //QuadTree::vertices.size()-4
            South  = {(QuadTree::vertices[this->quad->c2] + QuadTree::vertices[this->quad->c3])/2.f}, //QuadTree::vertices.size()-3
            West   = {(QuadTree::vertices[this->quad->c3] + QuadTree::vertices[this->quad->c0])/2.f}, //wIndex
            Center = {(QuadTree::vertices[this->quad->c0] + QuadTree::vertices[this->quad->c2])/2.f}; //QuadTree::vertices.size()-1
  GLuint i = 0,
  nIndex = -1,
  eIndex = -1,
  sIndex = -1,
  wIndex = -1,
  cIndex = -1;
  for(auto &vertex : QuadTree::vertices)
  {
    if(vertex == North){
      nIndex = i;
    }if(vertex == East){
      eIndex = i;
    }if(vertex == South){
      sIndex = i;
    }if(vertex == West){
      wIndex = i;
    }if(vertex == Center){
      cIndex = i;
    }if(nIndex>-1u&&eIndex>-1u&&sIndex>-1u&&wIndex>-1u&&cIndex>-1u){
      break;
    }
    i++;
  }
  if(nIndex==-1u)
  {
    QuadTree::vertices.push_back(North);
    nIndex = (int)QuadTree::vertices.size() - 1;
  }
  if(eIndex==-1u)
  {
    QuadTree::vertices.push_back(East);
    eIndex = (int)QuadTree::vertices.size() - 1;
  }
  if(sIndex==-1u)
  {
    QuadTree::vertices.push_back(South);
    sIndex = (int)QuadTree::vertices.size() - 1;
  }
  if(wIndex==-1u)
  {
    QuadTree::vertices.push_back(West);
    wIndex = (int)QuadTree::vertices.size() - 1;
  }
  if(cIndex==-1u)
  {
    QuadTree::vertices.push_back(Center);
    cIndex = (int)QuadTree::vertices.size() - 1;
  }

  //Quad *nw = new Quad;  nw->c0 = this->quad->c0;    nw->c1 = nIndex;            nw->c2 = cIndex;                nw->c3 = wIndex;
  //Quad *ne = new Quad;  ne->c0 = nIndex;            ne->c1 = this->quad->c1;    ne->c2 = eIndex;                ne->c3 = cIndex;
  //Quad *se = new Quad;  se->c0 = cIndex;            se->c1 = eIndex;            se->c2 = this->quad->c2;        se->c3 = sIndex;
  //Quad *sw = new Quad;  sw->c0 = wIndex;            sw->c1 = cIndex;            sw->c2 = sIndex;                sw->c3 = this->quad->c3;

  this->nw = std::make_unique<QuadTree>(std::make_unique<Quad>(this->quad->c0, nIndex, cIndex, wIndex));
  this->ne = std::make_unique<QuadTree>(std::make_unique<Quad>(nIndex,this->quad->c1,eIndex,cIndex));
  this->sw = std::make_unique<QuadTree>(std::make_unique<Quad>(cIndex,eIndex,this->quad->c2,sIndex));
  this->se = std::make_unique<QuadTree>(std::make_unique<Quad>(wIndex,cIndex,sIndex,this->quad->c3));

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
  GLuint leftover=0;
  for(GLuint i=0;i<lod;i++)
  {
    GLuint boundary=QuadTree::quadTreeList.size();
    for(;leftover<boundary;leftover++)
    {
      QuadTree::quadTreeList[leftover]->split();
    }
  }
}


void QuadTree::instanceNoise() {
  vector<float> testNoise;

  auto start = std::chrono::high_resolution_clock::now();
  for(auto &vertex : QuadTree::vertices)
  {
    testNoise.push_back(Simplex::iqfBm(vertex));
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end-start;
  std::cout << "CPU fBm: " << diff.count() << "s\n";
}

void QuadTree::instanceNoiseR(int start, int end) {
  vector<float> testNoise;
  std::vector<glm::vec3> transformed;

  for(int i=start;i<end;i++)
  {
    glm::vec3 tmp = QuadTree::vertices.at(i);
    glm::vec3 sphere = glm::normalize(tmp);
    sphere = glm::mix(tmp, sphere*(float)RADIUS,1.f)*10.f;
    testNoise.push_back(Simplex::iqfBm(sphere));
    transformed.push_back(sphere);
  }
}

void QuadTree::threadedInstanceNoise(){
  int sizeT = QuadTree::vertices.size();

  std::thread fst (instanceNoiseR,0,sizeT/8);     // spawn new thread that calls foo()
  std::thread snd (instanceNoiseR,sizeT/8+1,sizeT/4);
  std::thread trd (instanceNoiseR,sizeT/4+1,sizeT*3/8);
  std::thread fth (instanceNoiseR,sizeT*3/8+1,sizeT/2);     // spawn new thread that calls foo()
  std::thread ftt (instanceNoiseR,sizeT/2+1,sizeT*5/8);
  std::thread sth (instanceNoiseR,sizeT*5/8+1,sizeT*3/4);     // spawn new thread that calls foo()
  std::thread svt (instanceNoiseR,sizeT*3/4+1,sizeT*7/8);
  std::thread eth (instanceNoiseR,sizeT*7/8+1,sizeT-1);

  fst.join();
  snd.join();
  trd.join();
  fth.join();
  ftt.join();
  sth.join();
  svt.join();
  eth.join();
}
/*
void QuadTree::nSplit(){

  if(this->quad->split)
    return;

  glm::vec3 center = {(QuadTree::vertices[this->quad->c0] + QuadTree::vertices[this->quad->c2])/2.f},
            north,
            east,
            south,
            west;
  GLuint i = 0,
  nIndex = -1,
  eIndex = -1,
  sIndex = -1,
  wIndex = -1,
  cIndex = -1;

  if(this->North.second != nullptr)
    nIndex = this->North.second->quad->c3;
  else
  {
    north  = {(QuadTree::vertices[this->quad->c0] + QuadTree::vertices[this->quad->c1])/2.f}; //nIndex
    QuadTree::vertices.push_back(north);
    nIndex = (int)QuadTree::vertices.size() - 1;
  }

  if(this->East.second != nullptr)
    eIndex = this->East.second->quad->c0;
  else
  {
    east   = {(QuadTree::vertices[this->quad->c1] + QuadTree::vertices[this->quad->c2])/2.f}; //QuadTree::vertices.size()-4
    QuadTree::vertices.push_back(east);
    eIndex = (int)QuadTree::vertices.size() - 1;
  }

  if(this->South.second != nullptr)
    sIndex = this->South.second->quad->c0;
  else
  {
    south  = {(QuadTree::vertices[this->quad->c2] + QuadTree::vertices[this->quad->c3])/2.f}; //QuadTree::vertices.size()-4
    QuadTree::vertices.push_back(south);
    eIndex = (int)QuadTree::vertices.size() - 1;
  }

  if(this->West.second != nullptr)
    wIndex = this->West.second->quad->c1;
  else
  {
    west   = {(QuadTree::vertices[this->quad->c3] + QuadTree::vertices[this->quad->c0])/2.f}; //QuadTree::vertices.size()-4
    QuadTree::vertices.push_back(west);
    wIndex = (int)QuadTree::vertices.size() - 1;
  }

  QuadTree::vertices.push_back(center);
  cIndex = (int)QuadTree::vertices.size() - 1;

  this->nw = std::make_unique<QuadTree>(std::make_unique<Quad>(this->quad->c0, nIndex, cIndex, wIndex));
  this->ne = std::make_unique<QuadTree>(std::make_unique<Quad>(nIndex,this->quad->c1,eIndex,cIndex));
  this->sw = std::make_unique<QuadTree>(std::make_unique<Quad>(cIndex,eIndex,this->quad->c2,sIndex));
  this->se = std::make_unique<QuadTree>(std::make_unique<Quad>(wIndex,cIndex,sIndex,this->quad->c3));

  this->nw->parent =   this->ne->parent =   this->sw->parent =   this->se->parent = this;

  QuadTree* tns = this->North.second;
  QuadTree* tss = this->South.second;
  QuadTree* tws = this->West.second;
  QuadTree* tes = this->East.second;

  if(tns == nullptr)
    tns = this->North.second;
  if(tss == nullptr)
    tss = this->South.second;
  if(tws == nullptr)
    tws = this->West.second;
  if(tes == nullptr)
    tes = this->East.second;

  this->nw->East = Neighbor(*ne,nullptr); this->nw->South = {this->sw,nullptr}; this->nw->West = {this->West.second,nullptr}; this->nw->North = {this->North.second,nullptr};
  this->ne->East = {this->East.second,nullptr};this->ne->South = {this->se,nullptr};this->ne->West = {this->nw,nullptr};this->ne->North = {tns,nullptr};
  this->sw->East = {this->se,nullptr}; this->sw->South = {this->South.second,nullptr}; this->sw->West = {tws,nullptr}; this->sw->North = {this->nw,nullptr};
  this->se->East = {tes,nullptr}; this->se->South = {tss,nullptr}; this->se->West = {this->sw,nullptr}; this->se->North = {this->ne,nullptr};

  if(this->North.second == nullptr)
    this->North.second->South = {this->nw,this->ne};
  if(this->East.second == nullptr)
    this->East.second->West = {this->ne,this->se};
  if(this->South.second == nullptr)
    this->South.second->North = {this->sw,this->se};
  if(this->West.second == nullptr)
    this->West.second->East = {this->nw,this->sw};

  this->quad->split = true;
}
*/
