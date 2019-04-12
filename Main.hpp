#include <iostream>
#include <string>
#include <chrono>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <typeinfo>
#include <cmath>

#include "SOIL.h"
#include "simplex.h"
#include "Utils.hpp"
#include "LoadShaders.hpp"
#include "Camera.h"
#include "Planet.hpp"
#include "QuadTree.hpp"
#include "NoiseFeedback.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#define QTDTEXTURAS 5
#define HEIGHT 1024
#define WIDTH   1280

using namespace glm;
using namespace std;


GLFWwindow* window;
const char* projectTitle = "World Renderer";

GLuint VertexArrayID, feedbackVAO;

float amp;
glm::vec3 lightPos;

GLuint tessellationShader;
GLuint transformFeedbackShader;
GLuint activeShader;

const char* filenames[QTDTEXTURAS] = {"texture.png",
                                    "detail.png",
                                    "normalmap.png"};

int width, height, nrChannels;


GLuint MatrixID, ModelMatrixID, ViewMatrixID, ProjectionMatrixID,
     TessLevelInnerID, TessLevelOuterID, radiusID, viewPosID, enableTessID;


vector<unsigned short> indices;
vector<GLfloat> vertices;
vector<GLfloat> texcoord;

bool tIsPressed, pIsPressed, enablePolygon, cIsPressed, enableCull, plusIsPressed, minusIsPressed, shiftMinusIsPressed, shiftPlusIsPressed, mIsPressed, pos2IsPressed, pos2 = true, noise2IsPressed, noise=true;

static GLuint index = 32.0;
static GLfloat meshSize = 128.0;


int initGL();
int init();
void createBuffer();
void bindBuffer();
void deleteBuffers();
void clearVectors();
void createVerticesIndexes();
void disableVertexAttribs();
void createProgram();
void deleteProgram();
void createTextures();
void setUnifLoc();
void setUnif();
void pressButtons();
void draw();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void swapBuffers();
