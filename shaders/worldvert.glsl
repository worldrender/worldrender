#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in float noise;

uniform float radius;
uniform mat4 MVP;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;
out float vertNoise;

void main() {

	vPosition = position;
    gl_Position = vec4(position, 1.0);
	vNormal = normalize(vPosition);
    vertNoise = noise;
	vPosition = vPosition;

  vColor = vec4(vNormal, 1.0);
}
