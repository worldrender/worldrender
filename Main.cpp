#include <iostream>
#include <string>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "include/LoadShaders.hpp"
#include "include/Planet.hpp"
#include "include/QuadTree.hpp"
#include "include/NoiseFeedback.hpp"
#include "include/Textures.hpp"
#include "include/Camera.hpp"
#include "include/Utils.hpp"

const vec3 cubeVerts::v0 = vec3(-1,-1,-1);
const vec3 cubeVerts::v1 = vec3(1,-1,-1);
const vec3 cubeVerts::v2 = vec3(1,1,-1);
const vec3 cubeVerts::v3 = vec3(-1,1,-1);
const vec3 cubeVerts::v4 = vec3(-1,-1,1);
const vec3 cubeVerts::v5 = vec3(1,-1,1);
const vec3 cubeVerts::v6 = vec3(1,1,1);
const vec3 cubeVerts::v7 = vec3(-1,1,1);

#include "include/Atmosphere.hpp"
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.hpp"

using namespace glm;
using namespace std;

GLFWwindow * window;

bool adapt = true;
bool unif = false;
bool geom = false;

vec4 color = vec4(1.f, 1.f, 1.f, 1.0f);

Planet * planet;
chrono::duration < double > diff;
GLuint feedbackVAO, skyboxVAO, skyboxVBO, cubemapTexture;
GLuint activeShader, skyboxShader, transformFeedbackShader;

Camera planetCamera(glm::vec3(-1800.f, 200.f, 200.0f));

int main(int argv, char ** argc) {
  init();

  createProgram();
  createPlanet();
  setSkybox();
  createBuffer();
  applyingTextures();

  /**ATMOSFERA**/
  bufferAtmosphere();
  /**ATMOSFERA**/

  const vector<GLuint> buffers  = {Atmosphere::VBO, Planet::VBO, Planet::VNI, skyboxVBO},
                       aob      = {Atmosphere::VAO, Planet::VAO, skyboxVAO, feedbackVAO};
  planetCamera.setPlanet(planet);

  do {
    // Clear the screen
    chrono::high_resolution_clock::time_point currentFrame = chrono::high_resolution_clock::now();
    chrono::duration < float > diff = currentFrame - planetCamera.lastFrame;
    planetCamera.deltaTime = diff.count();
    planetCamera.lastFrame = currentFrame;

    planetCamera.pressButtons();

    setUniforms();
//    planetCamera.calculateFrustum();
//    updateBuffer();
    draw();

    swapBuffers();

  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

  glDeleteTextures(2, textures);
  deleteBuffers(buffers);
  deleteArrayBuffers(aob);

  deleteProgram(new GLuint[4]{Planet::shader,transformFeedbackShader,skyboxShader,Atmosphere::shader});

  glfwTerminate();

  return 0;
}

void initGL() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_DECORATED, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "World Renderer", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);

  gl::centerWindow(window, gl::getBestMonitor(window));

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

  Planet::shader            = LoadShaders("shaders/worldvert.glsl", "shaders/worldtesc.glsl", "shaders/worldtese.glsl", "shaders/worldfrag.glsl");

  skyboxShader            = LoadShaders("shaders/skyboxvert.glsl", "shaders/skyboxfrag.glsl");
  transformFeedbackShader = LoadShader("shaders/transform.glsl");

  /**ATMOSFERA**/
  Atmosphere::shader      = LoadShaders("shaders/atmospherevert.glsl", "shaders/atmospheretesc.glsl", "shaders/atmospheretese.glsl", "shaders/atmospherefrag.glsl");
  //Atmosphere::shader      = LoadShaders("shaders/atmospherevert.glsl", "shaders/atmospherefrag.glsl");
  activeShader = Planet::shader;
}

void createPlanet() {

  planet = new Planet(cubeVerts::v0, cubeVerts::v1, cubeVerts::v2, cubeVerts::v3, cubeVerts::v4, cubeVerts::v5, cubeVerts::v6, cubeVerts::v7, RADIUS);

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
  glGenVertexArrays(1, & Planet::VAO);
  glBindVertexArray(Planet::VAO);

  glGenBuffers(1, & Planet::VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Planet::VBO);
  glBufferData(GL_ARRAY_BUFFER, QuadTree::transformedVertices.size() * sizeof(glm::vec3), QuadTree::transformedVertices.data(), GL_DYNAMIC_DRAW);

  glGenBuffers(1, & Planet::VNI);
  glBindBuffer(GL_ARRAY_BUFFER, Planet::VNI);
  glBufferData(GL_ARRAY_BUFFER, QuadTree::transformedVertices.size() * sizeof(GLfloat), QuadTree::noises.data(), GL_STATIC_DRAW);

  glGenBuffers(1, & Planet::VAI);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Planet::VAI);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, QuadTree::indices.size() * sizeof(GLuint), QuadTree::indices.data(), GL_STATIC_DRAW);
}

void updateBuffer() {
  glBindBuffer(GL_ARRAY_BUFFER, Planet::VBO);
  glBufferData(GL_ARRAY_BUFFER, QuadTree::visibleVerts.size() * sizeof(glm::vec3), QuadTree::visibleVerts.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, Planet::VNI);
  glBufferData(GL_ARRAY_BUFFER, QuadTree::vNoises.size() * sizeof(GLfloat), QuadTree::vNoises.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Planet::VAI);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, QuadTree::indices.size() * sizeof(GLuint), QuadTree::indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void applyingTextures() {
  for (int i = 0; i < QTDTEXTURAS; i++) {

    glGenTextures(1, & textures[i]);

    glUseProgram(Planet::shader);

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
  glUseProgram(Planet::shader);

  glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
  glm::mat4 ViewMatrix = planetCamera.getViewMatrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glUniform3f(glGetUniformLocation(Planet::shader, "viewPos"), planetCamera.Position.x, planetCamera.Position.y, planetCamera.Position.z);

  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  //    glm::mat4 MVP = camera.getProjectionMatrix(WIDTH, HEIGHT) * camera.getViewMatrix() * glm::mat4(1.0);

  glUniformMatrix4fv(glGetUniformLocation(Planet::shader, "MVP"), 1, GL_FALSE, & MVP[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(Planet::shader, "model"), 1, GL_FALSE, & ModelMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(Planet::shader, "view"), 1, GL_FALSE, & ViewMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(Planet::shader, "projection"), 1, GL_FALSE, & ProjectionMatrix[0][0]);
  glUniform1f(glGetUniformLocation(Planet::shader, "radius"), planet -> getRadius());
  glUniform1f(glGetUniformLocation(Planet::shader, "scale"), SCALE);
  glUniform1i(glGetUniformLocation(Planet::shader, "tess"), planetCamera.enableTess);
  glUniform1i(glGetUniformLocation(Planet::shader, "wireframe"), planetCamera.enablePolygon);
  glUniform1i(glGetUniformLocation(Planet::shader, "pTexture"), 0);
  glUniform1i(glGetUniformLocation(Planet::shader, "dTexture"), 1);
  glUniform1i(glGetUniformLocation(Planet::shader, "nTexture"), 2);
}

void draw() {

    glBindVertexArray(Planet::VAO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPatchParameteri(GL_PATCH_VERTICES, 3);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, Planet::VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void * ) 0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, Planet::VNI);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void * ) 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Planet::VAI);

  if (activeShader == Planet::shader) {
    glDrawElements(GL_PATCHES, QuadTree::indices.size(), GL_UNSIGNED_INT, (void * ) 0);
  } else {
    glDrawElements(GL_TRIANGLES, QuadTree::indices.size(), GL_UNSIGNED_INT, (void * ) 0);
  }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);

  // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    gl::disableCullFace();
    glUseProgram(skyboxShader);
    glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(planetCamera.getViewMatrix())); // remove translation from the view matrix
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, & ViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, & ProjectionMatrix[0][0]);
    glUniform1i(glGetUniformLocation(skyboxShader, "wireframe"), planetCamera.enablePolygon);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    gl::enableCullFace();
    glDepthFunc(GL_LESS); // set depth function back to default
    glUseProgram(0);

    QuadTree::quadTreeList.clear();

    if(planetCamera.enablePolygon&&!planetCamera.disableAtm)
    {
      /**ATMOSFERA**/
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glFrontFace(GL_CW);
      renderAtmosphere(Atmosphere::arrayIndex, 1, 0);
      glFrontFace(GL_CCW);

      for(GLuint i=0;i<Atmosphere::glow;i++)
        renderAtmosphere(Atmosphere::arrayIndex, 1, i);
      glDisable(GL_BLEND);
      /**ATMOSFERA**/
    }
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
  glUseProgram(0);
}

/**ATMOSFERA**/
void bufferAtmosphere(){
    // atmosphere VAO
  glGenVertexArrays(1, &Atmosphere::VAO);
  glGenBuffers(1, &Atmosphere::VBO);
  glGenBuffers(1, &Atmosphere::indices);

  glBindVertexArray(Atmosphere::VAO);
  glUseProgram(skyboxShader);

  glBindBuffer(GL_ARRAY_BUFFER, Atmosphere::VBO);
  glBufferData(GL_ARRAY_BUFFER, Atmosphere::vertices.size() * sizeof(glm::vec3), &Atmosphere::vertices[0], GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Atmosphere::indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Atmosphere::arrayIndex.size() * sizeof(GLuint), &Atmosphere::arrayIndex[0], GL_DYNAMIC_DRAW);

  glUseProgram(0);

}

void renderAtmosphere(const vector<GLuint>& w_indices, bool io, int size){

  glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
  glm::mat4 ViewMatrix = planetCamera.getViewMatrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glBindVertexArray(Atmosphere::VAO);

  glUseProgram(Atmosphere::shader);
  glBindBuffer(GL_ARRAY_BUFFER, Atmosphere::VBO);
  glBufferData(GL_ARRAY_BUFFER, Atmosphere::vertices.size() * sizeof(glm::vec3), &Atmosphere::vertices[0], GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Atmosphere::indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, w_indices.size() * sizeof(GLuint), &w_indices[0], GL_DYNAMIC_DRAW);

  glUniformMatrix4fv(glGetUniformLocation(Atmosphere::shader, "MVP"), 1, GL_FALSE, & MVP[0][0]);
  glUniform1f(glGetUniformLocation(Atmosphere::shader, "radius"), planet -> getRadius()*1.05);
  glUniform1f(glGetUniformLocation(Atmosphere::shader, "scale"), SCALE);
  glUniform1i(glGetUniformLocation(Atmosphere::shader, "io"), io);
  glUniform1i(glGetUniformLocation(Atmosphere::shader, "size"), size);
  glUniformMatrix4fv(glGetUniformLocation(Atmosphere::shader, "model"), 1, GL_FALSE, & ModelMatrix[0][0]);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, Atmosphere::VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Atmosphere::indices);

  glBindVertexArray(Atmosphere::VAO);
  glPatchParameteri(GL_PATCH_VERTICES, 3);
  glDrawElements(GL_PATCHES, w_indices.size(), GL_UNSIGNED_INT, (void * ) 0);

  glDisableVertexAttribArray(0);

  glUseProgram(0);

}
/**ATMOSFERA**/

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(faces[0].c_str(), &width, &height, &nrChannels, 0);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    stbi_image_free(data);

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

void deleteBuffers(const vector<GLuint> params) {
  for(const GLuint &abo : params)
    glDeleteBuffers(1, &abo);
}

void deleteArrayBuffers(const vector<GLuint> params){
  for(const GLuint &abo : params)
    glDeleteVertexArrays(1, &abo);
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
