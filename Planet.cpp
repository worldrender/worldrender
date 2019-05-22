#include "include/QuadTree.hpp"
#include "include/Planet.hpp"
#include <iostream>


using namespace std;
Planet::Planet():
  cube{nullptr}
{

}

/**
    v4--------v5
   /|        /|
  / |       / |
 v0--------v1 |
 |  |      |  |
 |  v7-----|--v6
 | /       | /
 |/        |/
 v3--------v2

**/
Planet::Planet(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
       glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 v7,
       GLfloat radius)
{
    for (auto v : {v0, v1, v2, v3, v4, v5, v6, v7})
      QuadTree::verts.addVertex(v);

    QuadTree* quadtree0 = new QuadTree(std::make_unique<Quad>(0,1,2,3),nullptr);
    QuadTree* quadtree1 = new QuadTree(std::make_unique<Quad>(1,5,6,2),nullptr);
    QuadTree* quadtree2 = new QuadTree(std::make_unique<Quad>(7,6,5,4),nullptr);
    QuadTree* quadtree3 = new QuadTree(std::make_unique<Quad>(4,0,3,7),nullptr);
    QuadTree* quadtree4 = new QuadTree(std::make_unique<Quad>(4,5,1,0),nullptr);
    QuadTree* quadtree5 = new QuadTree(std::make_unique<Quad>(3,2,6,7),nullptr);

    Cube *cube = new Cube();
    cube->Front = quadtree0; cube->Back = quadtree2;
    cube->Left = quadtree1; cube->Right = quadtree3;
    cube->Top = quadtree4; cube->Bottom = quadtree5;

    this->cube = cube;
    this->radius = radius;
}

Planet::Planet(Cube* cube, GLfloat radius)
{
    this->cube = cube;
    this->radius = radius;
}

GLfloat Planet::getRadius(){
    return this->radius;
}
