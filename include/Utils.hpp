#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define RADIUS 7.f
#define SCALE 100.f
#define LODVALUE 6 //safe number is 6
#define PositionSlot 0u
#define WIDTH 1280u
#define HEIGHT 980u
#define ASPECT_RATIO (float)WIDTH/(float)HEIGHT
#define meshSize 128
#define PLANET_SCALE 1.f
#define PI 3.14f
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MIN(a, b) (((b) < (a)) ? (b) : (a))

#define YAW           -8.f
#define PITCH         -8.f
#define SPEED          2.5f
#define SENSITIVITY    0.1f
#define ZOOM           45.0f

//renderAtmosphere
//draw
//setUniforms

using namespace std;
using namespace glm;

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

extern int enableTess;
extern GLuint frame_index;

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

  inline void backCullFace()
  {
    glCullFace(GL_BACK);
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

  inline void consolePosition(glm::vec3 pos)
  {
    std::cout << pos.x << "," <<
                 pos.y << "," <<
                 pos.z << "," << std::endl;
  }

  inline GLFWmonitor* getBestMonitor(GLFWwindow *window)
  {
    int monitorCount;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

    if (!monitors)
      return NULL;

    int windowX, windowY, windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetWindowPos(window, &windowX, &windowY);

    GLFWmonitor *bestMonitor = NULL;
    int bestArea = 0;

    for (int i = 0; i < monitorCount; ++i)
    {
      GLFWmonitor *monitor = monitors[i];

      int monitorX, monitorY;
      glfwGetMonitorPos(monitor, &monitorX, &monitorY);

      const GLFWvidmode *mode = glfwGetVideoMode(monitor);
      if (!mode)
        continue;

      int areaMinX = MAX(windowX, monitorX);
      int areaMinY = MAX(windowY, monitorY);

      int areaMaxX = MIN(windowX + windowWidth, monitorX + mode->width);
      int areaMaxY = MIN(windowY + windowHeight, monitorY + mode->height);

      int area = (areaMaxX - areaMinX) * (areaMaxY - areaMinY);

      if (area > bestArea)
      {
        bestArea = area;
        bestMonitor = monitor;
      }
    }

      return bestMonitor;
  }
  inline void centerWindow(GLFWwindow *window, GLFWmonitor *monitor)
  {
    if (!monitor)
      return;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
      return;

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwSetWindowPos( window,
                      monitorX + (mode->width - windowWidth) / 2,
                      monitorY + (mode->height - windowHeight) / 2);
    glfwSetCursorPos (window,
                      monitorX + (mode->width - windowWidth) / 2,
                      monitorY + (mode->height - windowHeight) / 2);
  }
}

class cubeVerts{
  public:
    static const vec3 v0;// = vec3(-1,-1,-1);
    static const vec3 v1;// = vec3(1,-1,-1);
    static const vec3 v2;// = vec3(1,1,-1);
    static const vec3 v3;// = vec3(-1,1,-1);
    static const vec3 v4;// = vec3(-1,-1,1);
    static const vec3 v5;// = vec3(1,-1,1);
    static const vec3 v6;// = vec3(1,1,1);
    static const vec3 v7;// = vec3(-1,1,1);
  private:
    void * operator new      (size_t);
    void * operator new[]    (size_t);
    void   operator delete   (void *);
    void   operator delete[] (void*);
    cubeVerts(){}
    ~cubeVerts(){}
};

const vec3 initialPos = vec3(-1800.f, 200.f, 200.0f);

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
void deleteArrayBuffers(const vector<GLuint> params);
void deleteProgram(const GLuint programs[]);
void deleteBuffers(const vector<GLuint> params);
void CPUfbm();
void setSkybox();
void updateBuffer();
unsigned int loadCubemap(std::vector<std::string> faces);

#endif
