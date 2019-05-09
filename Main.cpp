#include <iostream>
#include <string>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "LoadShaders.hpp"
#include "Planet.hpp"
#include "QuadTree.hpp"
#include "NoiseFeedback.hpp"
#include "Textures.hpp"
#include "Camera.hpp"
#include "Utils.hpp"
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

using namespace glm;
using namespace std;

GLFWwindow * window;

bool adapt = true;
bool unif = false;
bool geom = false;

vec4 color = vec4(0.5f, 0.5f, 0.8f, 1.0f);

Planet * planet;
chrono::duration < double > diff;
GLuint VertexArrayID, feedbackVAO, vertexbuffer, noiseBuffer, elementbuffer;
unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
GLuint planetShader, skyboxShader, activeShader, transformFeedbackShader, cullingShader;
int enableTess = 0;

Camera planetCamera(glm::vec3(-120.f, 780.f, 0.0f));

int main(int argv, char ** argc) {
  init();

  createProgram();
  createPlanet();
  setSkybox();
  createBuffer();
  applyingTextures();

  do {
    // Clear the screen
    glBindVertexArray(VertexArrayID);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    planetCamera.pressButtons();

    setUniforms();

    draw();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    swapBuffers();

  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

  deleteBuffers();

  deleteProgram(new GLuint[2]{planetShader,transformFeedbackShader});

  //CPUfbm();
  glfwTerminate();
  //extras();

  return 0;
}

void initGL() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "World Renderer", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    cout << "Failed to initialize GLEW\n";
    glfwTerminate();
    exit(-1);
  }

}

void init() {
  initGL();

  gl::enableDepthTest();

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwPollEvents();
  glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

  gl::clearColor(color);

  // Accept fragment if it closer to the camera than the former one
  gl::lessDepthFunction();

  // Cull triangles which normal is not towards the camera
  gl::enableCullFace();
  gl::frontCullFace();
}

void createProgram() {
  planetShader = LoadShaders("worldvert.glsl", "worldtesc.glsl", "worldtese.glsl", "worldfrag.glsl");
  skyboxShader = LoadShaders("skybox.vs", "skybox.fs");
  transformFeedbackShader = LoadShader("transform.glsl");
  cullingShader = LoadShaders("culling.vs", "culling.gs", "culling.ps");
  activeShader = planetShader;
}

void createPlanet() {
  float auxX, auxY, auxZ;
  auxX = -1;
  auxY = -1;
  auxZ = -1;
  glm::vec3 v0 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = 1;
  auxY = -1;
  auxZ = -1;
  glm::vec3 v1 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = 1;
  auxY = 1;
  auxZ = -1;
  glm::vec3 v2 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = -1;
  auxY = 1;
  auxZ = -1;
  glm::vec3 v3 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = -1;
  auxY = -1;
  auxZ = 1;
  glm::vec3 v4 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = 1;
  auxY = -1;
  auxZ = 1;
  glm::vec3 v5 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = 1;
  auxY = 1;
  auxZ = 1;
  glm::vec3 v6 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  auxX = -1;
  auxY = 1;
  auxZ = 1;
  glm::vec3 v7 = vec3(auxX, auxY, auxZ)*PLANET_SCALE;

  planet = new Planet(v0, v1, v2, v3, v4, v5, v6, v7, RADIUS);

  QuadTree::verticalSplit(LODVALUE);

  feedbackBuffer();

  QuadTree::triangulator();

}

void feedbackBuffer() {
  glGenVertexArrays(1, & feedbackVAO);
  glBindVertexArray(feedbackVAO);
  auto start = std::chrono::high_resolution_clock::now();
  instanceNoise(transformFeedbackShader);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration < double > diff = end - start;
  std::cout << "Feedback throttle: " << diff.count() << "s\n";
  glBindVertexArray(0);
}

void createBuffer() {
  glGenVertexArrays(1, & VertexArrayID);
  glBindVertexArray(VertexArrayID);

  glGenBuffers(1, & vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(glm::vec3), transformedVertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, & noiseBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, noiseBuffer);
  glBufferData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(GLfloat), QuadTree::noises.data(), GL_STATIC_DRAW);

  glGenBuffers(1, & elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, QuadTree::indices.size() * sizeof(GLuint), QuadTree::indices.data(), GL_STATIC_DRAW);
}

void applyingTextures() {
  for (int i = 0; i < QTDTEXTURAS; i++) {

    glGenTextures(1, & textures[i]);

    glUseProgram(planetShader);

    glActiveTexture(GL_TEXTURE0 + i);

    glBindTexture(GL_TEXTURE_2D, textures[i]);
    int width, height, nrChannels;
    unsigned char *textData = stbi_load(filenames[i], & width, & height, & nrChannels, STBI_rgb_alpha);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textData);
    stbi_image_free(textData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
}

void setUniforms() {
  glUseProgram(planetShader);

  glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
  glm::mat4 ViewMatrix = planetCamera.getViewMatrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glUniform3f(glGetUniformLocation(planetShader, "viewPos"), planetCamera.Position.x, planetCamera.Position.y, planetCamera.Position.z);

  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  //    glm::mat4 MVP = camera.getProjectionMatrix(WIDTH, HEIGHT) * camera.getViewMatrix() * glm::mat4(1.0);

  glUniformMatrix4fv(glGetUniformLocation(planetShader, "MVP"), 1, GL_FALSE, & MVP[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "model"), 1, GL_FALSE, & ModelMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "view"), 1, GL_FALSE, & ViewMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "projection"), 1, GL_FALSE, & ProjectionMatrix[0][0]);
  glUniform1f(glGetUniformLocation(planetShader, "radius"), planet -> getRadius());
  glUniform1i(glGetUniformLocation(planetShader, "tess"), enableTess);
  glUniform1i(glGetUniformLocation(planetShader, "pTexture"), 0);
  glUniform1i(glGetUniformLocation(planetShader, "dTexture"), 1);
  glUniform1i(glGetUniformLocation(planetShader, "nTexture"), 2);
}

void draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPatchParameteri(GL_PATCH_VERTICES, 3);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void * ) 0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, noiseBuffer);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void * ) 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

  if (activeShader == planetShader) {
    glDrawElements(GL_PATCHES, QuadTree::indices.size(), GL_UNSIGNED_INT, (void * ) 0);
  } else {
    glDrawElements(GL_TRIANGLES, QuadTree::indices.size(), GL_UNSIGNED_INT, (void * ) 0);
  }

  // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    gl::disableCullFace();
    glUseProgram(skyboxShader);
    glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(planetCamera.getViewMatrix())); // remove translation from the view matrix
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, & ViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, & ProjectionMatrix[0][0]);

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    gl::enableCullFace();
    glDepthFunc(GL_LESS); // set depth function back to default
}

void setSkybox(){

    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    cubemapTexture = loadCubemap(skybox);

    glUseProgram(skyboxShader);
    glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void swapBuffers() {
  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void deleteProgram(const GLuint programs[]) {
  for(GLuint i=0;i<size(programs);i++)
    glDeleteProgram(programs[i]);
  delete programs;
}

void deleteBuffers() {
  glDeleteTextures(2, textures);
  glDeleteBuffers(1, & vertexbuffer);
  glDeleteBuffers(1, & elementbuffer);
  glDeleteVertexArrays(1, & VertexArrayID);
  glDeleteVertexArrays(1, & feedbackVAO);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    planetCamera.ProcessMouseScroll(yoffset);
}

void CPUfbm() {
  auto start = std::chrono::high_resolution_clock::now();
  QuadTree::threadedInstanceNoise();
  auto end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "CPU fBm: " << diff.count() << "s\n";
}

//void extras(){
//    GLuint warn[] = {TessLevelInnerID, TessLevelOuterID};
//    if(warn);
//    if(IndexCount);
//}
