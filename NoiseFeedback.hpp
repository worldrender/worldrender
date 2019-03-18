#ifndef NOISEFEEDBACK_H_
#define NOISEFEEDBACK_H_
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/gl.h>
using namespace glm;

struct InstancedNoise
{
  vec3 vertex;
  GLshort index;
  GLfloat noiseValue;
};

void instanceNoise(GLuint program);
#endif
