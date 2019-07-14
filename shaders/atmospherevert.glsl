#version 430 core

layout(location = 0) in vec3 position;

out vec3 vPosition;
out vec3 vNormal;

void main() {
	vPosition = position;
	vNormal = normalize(vPosition);
}
