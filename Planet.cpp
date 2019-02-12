#include "QuadTree.hpp"
#include "Planet.hpp"

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

    Quad quad0, quad1, quad2, quad3, quad4, quad5;
    quad0.c0 = 0; quad0.c1 = 1; quad0.c2 = 2; quad0.c3 = 3;
    quad1.c0 = 1; quad1.c1 = 5; quad0.c2 = 6; quad0.c3 = 2;
    quad2.c0 = 5; quad2.c1 = 4; quad2.c2 = 7; quad2.c3 = 6;
    quad3.c0 = 4; quad3.c1 = 0; quad3.c2 = 3; quad3.c3 = 7;
    quad4.c0 = 4; quad4.c1 = 5; quad4.c2 = 1; quad4.c3 = 0;
    quad5.c0 = 3; quad5.c1 = 2; quad5.c2 = 6; quad5.c3 = 7;


    QuadTree* quadtree0 = new QuadTree(&quad0);
    QuadTree* quadtree1 = new QuadTree(&quad1);
    QuadTree* quadtree2 = new QuadTree(&quad2);
    QuadTree* quadtree3 = new QuadTree(&quad3);
    QuadTree* quadtree4 = new QuadTree(&quad4);
    QuadTree* quadtree5 = new QuadTree(&quad5);

    Cube cube;
    cube.Front = quadtree0; cube.Back = quadtree2;
    cube.Left = quadtree1; cube.Right = quadtree3;
    cube.Top = quadtree4; cube.Bottom = quadtree5;

    this->cube = &cube;
    this->radius = radius;

}

Planet::Planet(Cube* cube, GLfloat radius)
{
    this->cube = cube;
    this->radius = radius;
}


