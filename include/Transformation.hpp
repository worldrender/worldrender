#ifndef TransformationATION_HPP_INCLUDED
#define TransformationATION_HPP_INCLUDED

#include <glm/gtc/quaternion.hpp>

class Transformation
{
public:
	Transformation();
	~Transformation();


	void update();

	void setPosition(glm::vec3 pos)             { this->pos = pos; }
	void setPosition(float x, float y, float z) { this->pos = glm::vec3(x, y, z); }
	void Translate(glm::vec3 translation)       { this->pos += translation; }
	void setRotation(glm::quat rotation)        { this->rotation = rotation; }
	void setRotation(glm::vec3 euler)           { this->rotation = glm::quat(euler); }
	void Rotate(glm::quat rotation)             { this->rotation *= rotation; }
	void Rotate(glm::vec3 euler)                { this->rotation *= glm::quat(euler); }
	void setScale(glm::vec3 scaleVector)        { this->scaleVector = scaleVector; }
	void plusScale(glm::vec3 scaleVector)           { this->scaleVector *= scaleVector; }

	const glm::mat4& getTransformation() const  { return this->transformMatrix; }
	const glm::vec3& getPosition() const        { return this->pos; }
	const glm::quat& getRotation() const        { return this->rotation; }
	const glm::vec3& getScale() const           { return this->scaleVector; }

	const glm::vec3& getForward() const         { return this->forward; }
	const glm::vec3& getUp() const              { return this->up; }
	const glm::vec3& getRight() const           { return this->right; }

private:
	glm::mat4 transformMatrix;

	glm::vec3 pos;
	glm::quat rotation;
	glm::vec3 scaleVector;

	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
};

#endif
