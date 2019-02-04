#include <vector>
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

#include "LoadShaders.hpp"
#include "controls.hpp"
extern glm::vec3 position;

bool adapt = true;
bool unif  = false;
bool geom = false;
static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static float TessLevelInner;
static float TessLevelOuter;
const char* const filename = "C:\\Users\\iagop\\Documents\\Pesquisa2018-20190123T160530Z-001\\Pesquisa2018\\text.bmp";


int main(int argv, char** argc){
    cout<<"Press J to get the Geometry Shader"<<endl;
    cout<<"Press K to get the Adapt Tessellation Shader"<<endl;
    cout<<"Press L to get the Uniform Tessellation Shader"<<endl;

    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1280, 1024, "IPSinewave_v6", NULL, NULL);
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

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, 1280/2, 1024/2);

	glClearColor(0.5f, 0.5f, 0.8f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
    //	if()
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programAdaptID = LoadShaders( "world.vert", "world.frag");

    GLuint MatrixID, ModelMatrixID, ViewMatrixID, ProjectionMatrixID,
    cameraPosIDX, cameraPosIDY, cameraPosIDZ, ampValue, octavesValue,
    lacunarityValue, LightID, TessLevelInnerID, TessLevelOuterID;


    vector<GLushort> indices;
    unsigned int aux1[36] = {
        0, 1, 2, 2, 3, 0, // top
        3, 2, 6, 6, 7, 3, // back
        7, 6, 5, 5, 4, 7, // bottom
        4, 5, 1, 1, 0, 4, // left
        4, 0, 3, 3, 7, 4, // right
        1, 5, 6, 6, 2, 1,};


    for(int i = 0; i< 36; i++){
        indices.push_back(aux1[i]);
    }

    vector<GLfloat> vertices;

    float auxX, auxY, auxZ;
    auxX = -1; auxY = -1; auxZ = -1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = 1; auxY = -1; auxZ = -1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = 1; auxY = 1; auxZ = -1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = -1; auxY = 1; auxZ = -1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = -1; auxY = -1; auxZ = 1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = 1; auxY = -1; auxZ = 1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = 1; auxY = 1; auxZ = 1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    auxX = -1; auxY = 1; auxZ = 1;
    vertices.push_back(auxX); vertices.push_back(auxY); vertices.push_back(auxZ);

    // Create the VBO for positions:
    GLuint vertexbuffer;
    //GLsizei stride = 2 * sizeof(float);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Create the VBO for indices:
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    // For speed computation
    TessLevelInner = 1.0f;
    TessLevelOuter = 4.0f;
    glm::vec3 camerapos = position;

    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programAdaptID);
        MatrixID             = glGetUniformLocation(programAdaptID, "MVP");
        ModelMatrixID        = glGetUniformLocation(programAdaptID, "M");
        ViewMatrixID         = glGetUniformLocation(programAdaptID, "V");
        ProjectionMatrixID   = glGetUniformLocation(programAdaptID, "P");
        cameraPosIDX         = glGetUniformLocation(programAdaptID, "px");
        cameraPosIDY         = glGetUniformLocation(programAdaptID, "py");
        cameraPosIDZ         = glGetUniformLocation(programAdaptID, "pz");
        ampValue             = glGetUniformLocation(programAdaptID, "amp");
        octavesValue         = glGetUniformLocation(programAdaptID, "oct");
        lacunarityValue      = glGetUniformLocation(programAdaptID, "lac");
        LightID              = glGetUniformLocation(programAdaptID, "LightPosition_worldspace");

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        float px = position.x; float py = position.y; float pz = position.z;

               //cout<<"     min = "<<minnn<<" e max = "<<maxxx<<endl;
        if (glfwGetKey( window, GLFW_KEY_U ) == GLFW_PRESS){
           glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey( window, GLFW_KEY_I ) == GLFW_PRESS){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
            glEnable(GL_CULL_FACE);
        }
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
            glDisable(GL_CULL_FACE);
        }
        if (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS){
//            oct = rand() % 7;
  //          cout<<oct<<endl;
        }
        if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_PRESS){
          //  lac = rand() % 11;
    //        cout<<lac<<endl;
        }

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
        glUniform1f(cameraPosIDX, px);
        glUniform1f(cameraPosIDY, py);
        glUniform1f(cameraPosIDZ, pz);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    //}
        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );

        // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    GLuint warn[] = {ampValue,octavesValue,lacunarityValue,LightID,TessLevelInnerID, TessLevelOuterID, (GLuint)camerapos.x};
    if(warn);
    if(IndexCount);

    return 0;
}
