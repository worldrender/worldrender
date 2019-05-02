#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/gl.h>

#define RADIUS 7
#define LODVALUE 6
#define PositionSlot 0u
#define WIDTH 1280u
#define HEIGHT 1024u
#define meshSize 128

#define YAW           -10.0f
#define PITCH         -10.0f
#define SPEED          2.5f
#define SENSITIVITY    0.1f
#define ZOOM           45.0f

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
