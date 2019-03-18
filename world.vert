#version 430 core

in vec3 position;

uniform float radius;
uniform mat4 MVP;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

void main() {
    vec3 sphereCoord = normalize(position);
	vPosition = mix(position, sphereCoord*radius, 1)*10;

	vNormal = normalize(vPosition);
	vNormal = -vNormal;

    vColor = vec4(position, 1.0);
    //gl_Position = MVP * vec4(mix(position, sphereCoord*radius, 1), 1.0);
}
