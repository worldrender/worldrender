#include "quads.hpp"

std::vector<glm::vec3> Quadtree::vertices;
std::vector<GLushort> Quadtree::indices;
std::vector<Quad> quadTreeList;

Quadtree::Quadtree():
    nw{nullptr},
    ne{nullptr},
    sw{nullptr},
    se{nullptr},
    quad{}
{
}

Quadtree::Quadtree(Quad quad):
    nw{nullptr},
    ne{nullptr},
    sw{nullptr},
    se{nullptr}
{
  this->quad = quad;
}

Quadtree::~Quadtree()
{
    delete nw;
    delete ne;
    delete sw;
    delete se;
}

void Quadtree::split()
{
  if(this->quad.split)
    return;
    quadTreeList.push_back()
  glm::vec3 North  = {(Quadtree::vertices[this->quad.c0]+Quadtree::vertices[this->quad.c1])/2.f}, //nIndex
            East   = {(Quadtree::vertices[this->quad.c1]+Quadtree::vertices[this->quad.c2])/2.f}, //Quadtree::vertices.size()-4
            South  = {(Quadtree::vertices[this->quad.c2]+Quadtree::vertices[this->quad.c3])/2.f}, //Quadtree::vertices.size()-3
            West   = {(Quadtree::vertices[this->quad.c3]+Quadtree::vertices[this->quad.c0])/2.f}, //wIndex
            Center = {(Quadtree::vertices[this->quad.c0]+Quadtree::vertices[this->quad.c2])/2.f}; //Quadtree::vertices.size()-1
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
    if(nIndex>-1&&eIndex>-1&&sIndex>-1&&wIndex>-1&&cIndex>-1)
      break;
    i++;
  }
  if(nIndex==-1)
  {
    Quadtree::vertices.push_back(North);
    nIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(eIndex==-1)
  {
    Quadtree::vertices.push_back(East);
    eIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(sIndex==-1)
  {
    Quadtree::vertices.push_back(South);
    sIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(wIndex==-1)
  {
    Quadtree::vertices.push_back(West);
    wIndex = (int)Quadtree::vertices.size() - 1;
  }
  if(cIndex==-1)
  {
    Quadtree::vertices.push_back(Center);
    cIndex = (int)Quadtree::vertices.size() - 1;
  }

  Quad nw = {this->quad.c0,nIndex,cIndex,wIndex},
       ne = {nIndex,this->quad.c1,eIndex,cIndex},
       se = {cIndex,eIndex,this->quad.c2,sIndex},
       sw = {wIndex,cIndex,sIndex,this->quad.c3};

  this->nw = new Quadtree(nw);
  this->ne = new Quadtree(ne);
  this->sw = new Quadtree(sw);
  this->se = new Quadtree(se);

  this->quad.split = true;
  quadTreeList.push_back(nw);
  quadTreeList.push_back(ne);
  quadTreeList.push_back(se);
  quadTreeList.push_back(sw);
}

void Quadtree::triangulator(Quad quad){
    if(quad.split){
       return;
    }
    indices.push_back(quad.c0);
    indices.push_back(quad.c1);
    indices.push_back(quad.c2);

    indices.push_back(quad.c1);
    indices.push_back(quad.c3);
    indices.push_back(quad.c2);
}
