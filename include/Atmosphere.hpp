#ifndef ATMOSPHERE_HPP_INCLUDED
#define ATMOSPHERE_HPP_INCLUDED

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "Utils.hpp"

using namespace std;
using namespace glm;

void bufferAtmosphere();
void renderAtmosphere(const vector<GLuint>& w_indices, bool io, int size);

class Atmosphere{
  public:
    static GLuint       shader;
    static const vector<vec3> vertices;
    static const vector<GLuint> arrayIndex;

    static GLuint        VBO;
    static GLuint        VAO;
    static GLuint        indices;
    static const GLuint  glow = 50;

  private:
    void * operator new      (size_t);
    void * operator new[]    (size_t);
    void   operator delete   (void *);
    void   operator delete[] (void*);
    Atmosphere(){}
    ~Atmosphere(){}
};

const vector<vec3> Atmosphere::vertices       {cubeVerts::v0, cubeVerts::v1, cubeVerts::v2, cubeVerts::v3,
                                               cubeVerts::v4, cubeVerts::v5, cubeVerts::v6, cubeVerts::v7};

const vector<GLuint> Atmosphere::arrayIndex    {0, 1, 2, 0, 2, 3,
                                                3, 2, 6, 3, 6, 7,
                                                7, 6, 5, 7, 5, 4,
                                                4, 5, 1, 4, 1, 0,
                                                5, 6, 2, 5, 2, 1,
                                                7, 4, 0, 7, 0, 3};


GLuint Atmosphere::shader;
GLuint Atmosphere::VAO;
GLuint Atmosphere::VBO;
GLuint Atmosphere::indices;




#endif
