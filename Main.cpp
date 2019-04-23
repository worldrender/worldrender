#include <iostream>
#include <string>
//#include "stb_image.h"

#include <cstdlib>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

#include "Utils.hpp"
#include "LoadShaders.hpp"
#include "controls.hpp"
#include "Planet.hpp"
#include "QuadTree.hpp"
#include "NoiseFeedback.hpp"
#include "Textures.h"
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

extern glm::vec3 position;
extern glm::vec3 direction;

bool adapt = true;
bool unif  = false;
bool geom = false;
static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static float TessLevelInner;
static float TessLevelOuter;

Planet* planet;
chrono::duration<double> diff;
GLuint VertexArrayID, feedbackVAO, vertexbuffer, noiseBuffer, elementbuffer;
GLuint tessShader, 	geomShader, activeShader, transformFeedbackShader;
bool enableTess = true;

int initGL();
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

void deleteProgram();
void deleteBuffers();
void CPUfbm();


int main(int argv, char** argc){
    init();

    createProgram();
    createPlanet();
    createBuffer();
    applyingTextures();
    bool enableTess = true;

    bool tIsPressed=0;

    do{
        // Clear the screen
        glBindVertexArray(VertexArrayID);


        if (glfwGetKey( window, GLFW_KEY_U ) == GLFW_PRESS)
           glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (glfwGetKey( window, GLFW_KEY_I ) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS)
            glEnable(GL_CULL_FACE);

        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS)
            glDisable(GL_CULL_FACE);

        bool tIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS);
        if (!tIsPressed && tIsCurrentlyPressed){
            enableTess = !enableTess;
            if(enableTess)
                activeShader = tessShader;
        }
        tIsPressed = tIsCurrentlyPressed;


        setUniforms();

        draw();

        disableVertexAttribs();
        swapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    deleteBuffers();
    deleteProgram();


    CPUfbm();
    glfwTerminate();

//    extras();

    return 0;
}

int initGL(){
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1280, 1024, "World Renderer", NULL, NULL);
	if( window == NULL ){
    cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cout<<"Failed to initialize GLEW\n";
		glfwTerminate();
		return -1;
	}

	return 0;
}

void init(){
    initGL();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, 1280/2, 1024/2);

    glClearColor(0.5f, 0.5f, 0.8f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

void createProgram(){
	tessShader = LoadShaders( "worldvert.glsl", "worldtesc.glsl", "worldtese.glsl", "worldfrag.glsl");
    transformFeedbackShader = LoadShader("transform.glsl");
    activeShader = tessShader;
}

void createPlanet(){
    float auxX, auxY, auxZ;
    auxX = -1; auxY = -1; auxZ = -1;
    glm::vec3 v0 = vec3(auxX, auxY, auxZ);

    auxX = 1; auxY = -1; auxZ = -1;
    glm::vec3 v1 = vec3(auxX, auxY, auxZ);

    auxX = 1; auxY = 1; auxZ = -1;
    glm::vec3 v2 = vec3(auxX, auxY, auxZ);

    auxX = -1; auxY = 1; auxZ = -1;
    glm::vec3 v3 = vec3(auxX, auxY, auxZ);

    auxX = -1; auxY = -1; auxZ = 1;
    glm::vec3 v4 = vec3(auxX, auxY, auxZ);

    auxX = 1; auxY = -1; auxZ = 1;
    glm::vec3 v5 = vec3(auxX, auxY, auxZ);

    auxX = 1; auxY = 1; auxZ = 1;
    glm::vec3 v6 = vec3(auxX, auxY, auxZ);

    auxX = -1; auxY = 1; auxZ = 1;
    glm::vec3 v7 = vec3(auxX, auxY, auxZ);

    planet = new Planet(v0, v1, v2, v3, v4, v5,v6,v7, RADIUS);

    QuadTree::verticalSplit(LODVALUE);

    feedbackBuffer();

    QuadTree::triangulator();

}

void feedbackBuffer(){
    glGenVertexArrays(1, &feedbackVAO);
    glBindVertexArray(feedbackVAO);
    auto start = std::chrono::high_resolution_clock::now();
    instanceNoise(transformFeedbackShader);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << "Feedback throttle: " << diff.count() << "s\n";
    glBindVertexArray(0);
}

void createBuffer(){
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(glm::vec3), transformedVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &noiseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, noiseBuffer);
    glBufferData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(GLfloat), QuadTree::noises.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, QuadTree::indices.size() * sizeof(GLushort), QuadTree::indices.data(), GL_STATIC_DRAW);
}

void applyingTextures(){

    for(int i = 0; i < QTDTEXTURAS; i++){

        glGenTextures(1, &textures[i]);

        glUseProgram(tessShader);

        glActiveTexture(GL_TEXTURE0 + i);

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        data = stbi_load(filenames[i], &width, &height, &nrChannels, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

void setUniforms(){
    glUseProgram(tessShader);
    computeMatricesFromInputs(window);
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
//    glm::mat4 MVP = camera.getProjectionMatrix(WIDTH, HEIGHT) * camera.getViewMatrix() * glm::mat4(1.0);

    glUniformMatrix4fv(glGetUniformLocation(tessShader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(tessShader, "model"), 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(tessShader, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(tessShader, "projection"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
    glUniform1f(glGetUniformLocation(tessShader, "radius"), planet->getRadius());
    glUniform1i(glGetUniformLocation(tessShader, "tess"), enableTess);
    glUniform3f(glGetUniformLocation(tessShader, "viewPos"),position.x,position.y,position.z);
    glUniform1f(glGetUniformLocation(tessShader, "px"), position.x);
    glUniform1f(glGetUniformLocation(tessShader, "py"), position.y);
    glUniform1f(glGetUniformLocation(tessShader, "pz"), position.z);
    glUniform1i(glGetUniformLocation(tessShader, "pTexture"), 0);
    glUniform1i(glGetUniformLocation(tessShader, "dTexture"), 1);
    glUniform1i(glGetUniformLocation(tessShader, "nTexture"), 2);
}

void draw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, noiseBuffer);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    if(activeShader == tessShader){
        glDrawElements(GL_PATCHES, QuadTree::indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }
    else{
        glDrawElements(GL_TRIANGLES, QuadTree::indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }
}
void disableVertexAttribs(){
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void swapBuffers(){
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void deleteProgram(){
    glDeleteProgram(tessShader);
    glDeleteProgram(geomShader);
}

void deleteBuffers(){
    glDeleteTextures(2, textures);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteVertexArrays(1, &feedbackVAO);
}

void CPUfbm(){
    auto start = std::chrono::high_resolution_clock::now();
    QuadTree::threadedInstanceNoise();
    auto end = std::chrono::high_resolution_clock::now();
    diff = end-start;
    std::cout << "CPU fBm: " << diff.count() << "s\n";
}


//void extras(){
//    GLuint warn[] = {TessLevelInnerID, TessLevelOuterID};
//    if(warn);
//    if(IndexCount);
//}