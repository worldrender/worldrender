#include "QuadTree.hpp"
#include "Planet.hpp"
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
    QuadTree::vertices.push_back(v0); QuadTree::vertices.push_back(v1);
    QuadTree::vertices.push_back(v2); QuadTree::vertices.push_back(v3);
    QuadTree::vertices.push_back(v4); QuadTree::vertices.push_back(v5);
    QuadTree::vertices.push_back(v6); QuadTree::vertices.push_back(v7);

    QuadTree* quadtree0 = new QuadTree(std::make_unique<Quad>(0,1,2,3));
    QuadTree* quadtree1 = new QuadTree(std::make_unique<Quad>(1,5,6,2));
    QuadTree* quadtree2 = new QuadTree(std::make_unique<Quad>(7,6,5,4));
    QuadTree* quadtree3 = new QuadTree(std::make_unique<Quad>(4,0,3,7));
    QuadTree* quadtree4 = new QuadTree(std::make_unique<Quad>(4,5,1,0));
    QuadTree* quadtree5 = new QuadTree(std::make_unique<Quad>(3,2,6,7));

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
