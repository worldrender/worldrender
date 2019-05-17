#include "include/QuadTree.hpp"
#include "include/Utils.hpp"
#include "include/simplex.h"
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
std::vector<glm::vec3> QuadTree::normals;
std::vector<GLfloat> QuadTree::noises;
std::vector<GLuint> QuadTree::indices;
std::vector<GLuint> QuadTree::normalIndices;
std::vector<QuadTree*> QuadTree::quadTreeList;
Verts QuadTree::verts;

static glm::vec3 lookup(GLuint idx)
{
  return QuadTree::verts.lookupIndexRequired(
      static_cast<Verts::index_type>(idx));
}

/**
 * Default constructor for the \class QuadTree class, it instantiates a quadtree without any children or quad coordinates
 */

QuadTree::QuadTree():
    quad{},
    nw{nullptr},
    sw{nullptr},
    ne{nullptr},
    se{nullptr}
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
    sw{nullptr},
    ne{nullptr},
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

  glm::vec3 North  = (lookup(this->quad->c0) + lookup(this->quad->c1)) / 2.f, //nIndex
            East   = (lookup(this->quad->c1) + lookup(this->quad->c2)) / 2.f, //QuadTree::vertices.size()-4
            South  = (lookup(this->quad->c2) + lookup(this->quad->c3)) / 2.f, //QuadTree::vertices.size()-3
            West   = (lookup(this->quad->c3) + lookup(this->quad->c0)) / 2.f, //wIndex
            Center = (lookup(this->quad->c0) + lookup(this->quad->c2)) / 2.f ; //QuadTree::vertices.size()-1

  GLuint nIndex = QuadTree::verts.addVertex(North).index;
  GLuint eIndex = QuadTree::verts.addVertex(East).index;
  GLuint sIndex = QuadTree::verts.addVertex(South).index;
  GLuint wIndex = QuadTree::verts.addVertex(West).index;
  GLuint cIndex = QuadTree::verts.addVertex(Center).index;

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
  for(GLuint i=0;i<QuadTree::verts.size();i++)
  {
    glm::vec3 tmp = lookup(static_cast<GLuint>(i));
    testNoise.push_back(Simplex::iqfBm(tmp));
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
    glm::vec3 tmp = lookup(static_cast<GLuint>(i));
    glm::vec3 sphere = glm::normalize(tmp);
    sphere = glm::mix(tmp, sphere*(float)RADIUS,1.f)*10.f;
    testNoise.push_back(Simplex::iqfBm(sphere));
    transformed.push_back(sphere);
  }
}

void QuadTree::threadedInstanceNoise(){
  int sizeT = QuadTree::verts.size();

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
