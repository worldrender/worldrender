#ifndef NOISEFEEDBACK_H_
#define NOISEFEEDBACK_H_
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>

using namespace glm;

struct InstancedNoise
{
  vec3 vertex;
  GLuint index;
  GLfloat noiseValue;
};

extern std::vector<InstancedNoise> instancedNoise;
void instanceNoise(GLuint shader);
#endif
