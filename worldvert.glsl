#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in float noise;

uniform float radius;
uniform mat4 MVP;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

void main() {

	vPosition = position*10;

	vNormal = normalize(vPosition);

	vPosition = vPosition + vNormal*noise*4;

    vColor = vec4(vNormal, 1.0);
    //gl_Position = MVP * vec4(mix(position, sphereCoord*radius, 1), 1.0);
}