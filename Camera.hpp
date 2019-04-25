#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <vector>
#include "Utils.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

bool tIsPressed, pIsPressed, cIsPressed, plusIsPressed, minusIsPressed, shiftMinusIsPressed, shiftPlusIsPressed, mIsPressed, pos2IsPressed, noise2IsPressed, pDownIsPressed = false, pUpIsPressed = false;
extern GLFWwindow* window;

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
static float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
bool firstMouse = true;
bool enablePolygon = true, enableCull = true,  CPUnoise = true,  noise=false;
bool modeMouse;
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix()
    {
      return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 getProjectionMatrix(int SCR_WIDTH, int SCR_HEIGHT)
    {
      GLfloat pLength = fAbs(glm::length(Position));

      return glm::perspective(glm::radians(this->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, (0.1f+pLength)/pLength, (1000.0f*pLength));
    }
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

    void pressButtons(){
        bool noiseIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_F ) == GLFW_PRESS);
        if (!noise2IsPressed && noiseIsCurrentlyPressed){
            noise = !noise;
        }
        noise2IsPressed = noiseIsCurrentlyPressed;

        bool pos2IsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_C ) == GLFW_PRESS);
        if (!pos2IsPressed && pos2IsCurrentlyPressed){
            CPUnoise = !CPUnoise;
        }
        pos2IsPressed = pos2IsCurrentlyPressed;

        bool mIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_M ) == GLFW_PRESS);
        if (!mIsPressed && mIsCurrentlyPressed){
            modeMouse = !modeMouse;
            if(modeMouse){
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwMakeContextCurrent(0);
                glfwWaitEventsTimeout(5000);
            }
            else{
                glfwMakeContextCurrent(window);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            //glfwPollEvents();
        }
        mIsPressed = mIsCurrentlyPressed;

        bool cIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_C ) == GLFW_PRESS);
        if (!cIsPressed && cIsCurrentlyPressed){
            enableCull = !enableCull;
            if(enableCull)
                gl::enableCullFace();
            else
                gl::disableCullFace();
        }
        cIsPressed = cIsCurrentlyPressed;

        bool tIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS &&
                                    !(glfwGetKey( window, GLFW_KEY_RIGHT_SHIFT ) == GLFW_PRESS || glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS));
        if (!tIsPressed && tIsCurrentlyPressed){
            enableTess = (enableTess+1)%3;
        }
        tIsPressed = tIsCurrentlyPressed;

        bool pIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_T ) == GLFW_PRESS &&
                                    (glfwGetKey( window, GLFW_KEY_RIGHT_SHIFT ) == GLFW_PRESS || glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS));
        if (!pIsPressed && pIsCurrentlyPressed){
            enablePolygon = !enablePolygon;
            if(enablePolygon)
                gl::polygonModeFBFill();
            else
                gl::polygonModeFBLine();
        }
        pIsPressed = pIsCurrentlyPressed;

        bool pDownIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS);
        if (!pDownIsPressed && pDownIsCurrentlyPressed){
            float sp = this->MovementSpeed/2;
            if(sp<0.5)
                sp = 0.5;
            this->MovementSpeed = sp;
        }
        pDownIsPressed = pDownIsCurrentlyPressed;

        bool pUpIsCurrentlyPressed = (glfwGetKey( window, GLFW_KEY_PAGE_UP ) == GLFW_PRESS);
        if (!pUpIsPressed && pUpIsCurrentlyPressed){
            float sp = this->MovementSpeed*1.5;
            if(sp>150)
                sp = 150;
            this->MovementSpeed = sp;

        }
        pUpIsPressed = pUpIsCurrentlyPressed;

        if ((glfwGetKey( window, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS)){

        }

        if ((glfwGetKey( window, GLFW_KEY_PAGE_UP ) == GLFW_PRESS)){

        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            this->ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            this->ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            this->ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            this->ProcessKeyboard(RIGHT, deltaTime);
    }

private:
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
#endif

