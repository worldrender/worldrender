#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>
#include "include/Camera.hpp"
#include "include/Frustum.hpp"
// Default camera values

bool tIsPressed,      pIsPressed,
     cIsPressed,      plusIsPressed,
     minusIsPressed,  mIsPressed,
     pos2IsPressed,   noise2IsPressed,
     pDownIsPressed = false, pUpIsPressed = false;
bool firstMouse = true;
bool enablePolygon = true, enableCull = true,  CPUnoise = true,  noise=false;
bool modeMouse;

float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
float deltaTime = 0.0f;
std::chrono::high_resolution_clock::time_point lastFrame = {};

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch):
  Front(glm::vec3(0.0f, 0.0f, 0.0f)),
  MovementSpeed(SPEED),
  MouseSensitivity(SENSITIVITY),
  Zoom(ZOOM)
{
  this->Position = position;
  this->WorldUp  = up;
  this->frustum  = new Frustum();
  this->Yaw      = yaw;
  this->Pitch    = pitch;
  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch):
  Front(glm::vec3(0.0f, 0.0f, 0.0f)),
  MovementSpeed(SPEED),
  MouseSensitivity(SENSITIVITY),
  Zoom(ZOOM)
{
  Position       = glm::vec3(posX, posY, posZ);
  this->frustum  = new Frustum();
  WorldUp        = glm::vec3(upX, upY, upZ);
  Yaw            = yaw;
  Pitch          = pitch;
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
  return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::getProjectionMatrix(int SCR_WIDTH, int SCR_HEIGHT)
{
  if(Far < 2460.f)
    Position = lastPosition.at(0);

  GLfloat fLength  = fAbs(Position.x);
          fLength += fAbs(Position.y);
          fLength += fAbs(Position.z);

  fLength = glm::length(Position);
  Far   = (fLength)*RADIUS/2;
//  if(Far>RADIUS*80)
//    Far *= RADIUS;
  Near  = 1/std::log(fLength);

  return glm::perspective(glm::radians(this->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, Near , Far);
}
// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
  lastPosition.push_back(Position);
  if(lastPosition.size()>3)
    lastPosition.erase(lastPosition.begin());
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
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw   += xoffset;
  Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch)
  {
  if (Pitch > 89.9f)
    Pitch = 89.9f;
  if (Pitch < -89.9f)
    Pitch = -89.9f;
  }

  // Update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
  if (Zoom >= 40.0f && Zoom <= 45.0f)
    Zoom -= yoffset;
  if (Zoom <= 40.0f)
    Zoom = 40.0f;
  if (Zoom >= 45.0f)
    Zoom = 45.0f;
}

void Camera::pressButtons()
{
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
    {
      gl::polygonModeFBLine();
    }

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
    if(sp>550)
      sp = 550;
    this->MovementSpeed = sp;

  }
  pUpIsPressed = pUpIsCurrentlyPressed;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    this->ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    this->ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    this->ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    this->ProcessKeyboard(RIGHT, deltaTime);
}

void Camera::calculateFrustum()
{
  frustum->SetCullTransform(glm::mat4(1.0f)); //inserir matriz de transformação
  frustum->SetToCamera(this);

  //frustum->Transform(space);
  frustum->Update();

  QuadTree::indices.clear();
  QuadTree::visibleVerts.clear();
  QuadTree::vNoises.clear();

  frustum->ContainsQuad(this->planet->getCube()->Back);
  frustum->ContainsQuad(this->planet->getCube()->Bottom);
  frustum->ContainsQuad(this->planet->getCube()->Front);
  frustum->ContainsQuad(this->planet->getCube()->Left);
  frustum->ContainsQuad(this->planet->getCube()->Right);
  frustum->ContainsQuad(this->planet->getCube()->Top);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    planetCamera.ProcessMouseMovement(xoffset, yoffset);
}

