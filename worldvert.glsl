#version 430 core

layout(location = 0) in vec3 position;

uniform float radius;
uniform mat4 MVP;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

void main() {

	vPosition = position;

	vNormal = normalize(vPosition);
	vNormal = -vNormal;

    vColor = vec4(vNormal, 1.0);
    //gl_Position = MVP * vec4(mix(position, sphereCoord*radius, 1), 1.0);
}
