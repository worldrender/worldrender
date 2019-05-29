#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <chrono>

#include "Utils.hpp"
#include "Transformation.hpp"
#include "Planet.hpp"

extern bool tIsPressed;
extern bool pIsPressed;
extern bool cIsPressed;
extern bool plusIsPressed;
extern bool minusIsPressed;
extern bool mIsPressed;
extern bool pos2IsPressed;
extern bool noise2IsPressed;
extern bool pDownIsPressed;
extern bool pUpIsPressed;
extern bool enablePolygon;
extern bool enableCull;
extern bool CPUnoise;
extern bool noise;
extern bool modeMouse;
extern bool firstMouse;

extern GLFWwindow* window;

extern float lastX;
extern float lastY;
extern float deltaTime;
extern std::chrono::high_resolution_clock::time_point lastFrame;

#define NEWCAMERA
#ifndef NEWCAMERA

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

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
    float Near;
    float Far;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(int SCR_WIDTH, int SCR_HEIGHT);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void Update(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void pressButtons();

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

extern Camera planetCamera;


void mouse_callback(GLFWwindow* window, double xpos, double ypos);


#else
class Camera
{
public:
	Camera();
	~Camera();

	void SetFarClippingPlane(float far)   { this->far = far; }
	void SetFieldOfView(float fov)        { this->fov = fov; }
	void SetOrthoSize(float size)         { this->size = size; }
	void SetNearClippingPlane(float near) { this->near = near; }

	void UsePerspectiveProjection()       { this->isPerspective = true; }
	void UseOrthographicProjection()      { this->isPerspective = false; }

	void Update(float deltaTime);

	glm::mat4 GetView()                   { return this->View; }
	glm::mat4 GetProj()                   { return this->Projection; }
	glm::mat4 GetViewInv()                { return this->ViewInverse; }
	glm::mat4 GetViewProj()               { return this->ViewProjection; }
	glm::mat4 GetViewProjInv()            { return this->ViewProjectionInverse; }
	float GetFOV()                        { return this->fov; }
	float GetFarPlane()                   { return this->far; }
	float GetNearPlane()                  { return this->near; }
	void processMouseScroll(float yoffset);

	void SetPlanet(Planet *planet)        { this->planet = planet; }

	Transformation* GetTransform()        { return this->transform; }

	float GetAltitude()                   { return this->altitude; }

  static float deltaX;
	static float deltaY;

private:

	Planet *planet = nullptr;
	float altitude = SCALE,
        latitude = 0.f,
	      longitude = 0.f,
	      rotationSpeed = 0.5f,
	      speed = SPEED,

	      far,
	      near,
	      fov,
	      size;

	Transformation *transform = nullptr;

	//Camera projection
	glm::mat4 View,
	        	Projection,
	         	ViewInverse,
		        ViewProjection,
		        ViewProjectionInverse;
	bool isPerspective;
};

extern Camera planetCamera;
#endif // NEWCAMERA

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
#endif

