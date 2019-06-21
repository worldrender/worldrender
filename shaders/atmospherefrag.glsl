#version 430 core

in vec4 vcColor;
in vec3 vcNormal;
in vec3 vcPos;

uniform bool wireframe;

uniform vec3 lightDir = vec3(-1, -0.3, 1);

uniform vec3 diffuse = vec3(1.0f, 0.5f, 0.2f);
uniform vec3 ambient = vec3(0.05f, 0.05f, 0.08f);

out vec4 fColor;


float fAbs(float t)
{
  return t>0 ? t : -t;
}

void main() {

  fColor = vec4(0.f, 0.f, 0.f, 1.f);
}


