#ifndef NOISEFEEDBACK_H_
#define NOISEFEEDBACK_H_
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/gl.h>


#include "QuadTree.hpp"
#include "LoadShaders.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include "Utils.hpp"


using namespace glm;

struct InstancedNoise
{
  vec3 vertex;
  GLfloat noiseValue;
};

extern std::vector<InstancedNoise> transformedData;
void instanceNoise(GLuint shader);
#endif
