#version 430 core

in vec3 position;

uniform mat4 MVP;

out vec4 vColor;

void main() {

	vColor = vec4(position, 1.0);

    gl_Position = MVP * vec4(position, 1.0);

}
