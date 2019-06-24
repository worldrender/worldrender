#version 430 core

uniform bool wireframe;

uniform float radius;
uniform float scale;
uniform bool io;

uniform vec3 viewPos;
uniform vec3 lightColor = vec3(0.1f, 0.1f, 0.2f);
uniform vec3 objectColor = vec3(0.5f, 0.6f, .9f);
uniform vec3 specular = vec3(0.4f, 0.5f, .9f);
uniform vec3 lightPos = vec3(-1, -0.3, 1);

in vec3 vcNormal;
in vec3 vcPos;

out vec4 fColor;


float fAbs(float t)
{
  return t>0 ? t : -t;
}

void main() {
  vec3 normal = normalize(vcNormal);

  float ambientStrength = 1;
  vec3 ambient = ambientStrength * lightColor;
  vec3 lightDir = normalize(lightPos*radius*scale*2.5 - vcPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * specular;


  vec3 result = (ambient + diffuse) * objectColor;

  fColor.rgb = result;
  fColor.a   = 0.5f;
  fColor.a  *= (fColor.b);

  if(io)
  {
    fColor.rgb = vec3(0.3f, 0.4f, .8f);
  }
}


