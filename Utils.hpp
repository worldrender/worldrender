#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/gl.h>

#define RADIUS 7
#define LODVALUE 6

template<typename T>
T fAbs(T t)
{
  return t>0 ? t : -t;
}

template<typename T>
T size(T* t)
{
  return (sizeof(t)/sizeof(t[0]));
}

static const GLuint PositionSlot = 0u;
static const GLuint WIDTH = 1280u;
static const GLuint HEIGHT = 1024u;
static const GLfloat meshSize = 128.0f;

extern bool enableTess;

extern std::vector<glm::vec3> transformedVertices;

namespace gl
{
  inline void clearColor(glm::vec4 color)
  {
    glClearColor(color.x,color.y,color.z,color.w);
  }
  inline void enableDepthTest()
  {
    glEnable(GL_DEPTH_TEST);
  }
  inline void enableCullFace()
  {
    glEnable(GL_CULL_FACE);
  }

  inline void disableCullFace()
  {
    glDisable(GL_CULL_FACE);
  }

  inline void frontCullFace()
  {
    glCullFace(GL_FRONT);
  }

  inline void lessDepthFunction()
  {
    glDepthFunc(GL_LESS);
  }

  inline void polygonModeFBFill()
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  inline void polygonModeFBLine()
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
}

void initGL();
void init();
void createProgram();
void createPlanet();
void feedbackBuffer();
void createBuffer();
void applyingTextures();
void setUniforms();
void draw();
void disableVertexAttribs();
void swapBuffers();
void deleteProgram(const GLuint programs[]);
void deleteBuffers();
void CPUfbm();

#endif
