#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <chrono>

#include "Frustum.hpp"
#include "Utils.hpp"
#include "Planet.hpp"

extern GLFWwindow* window;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Frustum;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position = initialPos;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    std::vector<glm::vec3> lastPosition;

    // Euler Angles
    float Yaw;
    float Pitch;
    float Near = 0.0001f;
    float Far = 2500.f;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float lastX;
    float lastY;
    float deltaTime;
    std::chrono::high_resolution_clock::time_point lastFrame;

    bool is0P            = false;
    bool is1P            = false;
    bool is2P            = false;
    bool is3P            = false;
    bool is4P            = false;
    bool cIsPressed      = false;
    bool plusIsPressed   = false;
    bool minusIsPressed  = false;
    bool mIsPressed      = false;
    bool pos2IsPressed   = false;
    bool isPUP           = false;
    bool pUpIsPressed    = false;
    bool firstMouse      = true;
    bool modeMouse       = false;
    bool enablePolygon   = true;
    bool enableCull      = true;
    bool disableAtm      = false;
    bool CPUnoise        = true;
    bool noise           = false;
    int  enableTess      = 0;

    // Constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.17f, .98f, -0.03f), float yaw = YAW, float pitch = PITCH);

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(int SCR_WIDTH, int SCR_HEIGHT);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void pressButtons();
    void calculateFrustum();

    void setFrustum(Frustum *frustum){this->frustum = frustum;}
    Frustum *getFrustum(){return this->frustum;}

    void setPlanet(Planet *planet){this->planet = planet;}

private:
    Planet *planet;
    Frustum *frustum;
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

extern Camera planetCamera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#endif

