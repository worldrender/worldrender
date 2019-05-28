#include "include/Transformation.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

Transformation::Transformation()
{
	this->transformMatrix = glm::mat4(1.0f);

	this->pos             = glm::vec3(0.f, 0.f, 0.f);
	this->rotation        = glm::quat();
	this->scaleVector     = glm::vec3(1.f, 1.f, 1.f);


	this->forward         = glm::vec3(0.f, 0.f, 1.f);
	this->right           = glm::vec3(1.f, 0.f, 0.f);
	this->up              = glm::vec3(0.f, 1.f, 0.f);
}

void Transformation::update()
{
	this->transformMatrix = glm::translate(this->pos)*glm::toMat4(this->rotation)*glm::scale(this->scaleVector);

	this->forward         = glm::normalize(this->rotation*glm::vec3(0, 0, 1));
	this->right           = glm::normalize(this->rotation*glm::vec3(1, 0, 0));
	this->up              = glm::normalize(glm::cross(this->forward, this->right));
}

Transformation::~Transformation()
{
}
