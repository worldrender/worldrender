#version 430 core

uniform bool wireframe;

uniform float radius;
uniform float scale;
uniform bool io;
uniform int size;

uniform vec3 viewPos;
uniform vec3 atmosphereColor = vec3(0.1f, 0.1f, 0.2f);
uniform vec3 mieScattering = vec3(0.5f, 0.6f, .9f);
uniform vec3 rayleighScattering = vec3(0.4f, 0.5f, .9f);
uniform vec3 sunRay = vec3(-1, -0.3, 1);

in vec3 vcNormal;
in vec3 vcPos;

out vec4 fColor;


float fAbs(float t)
{
  return t>0 ? t : -t;
}

void main() {
  vec3 normal = normalize(vcNormal);
  normal = normalize(normal);
  float sourceStrength = 1;
  vec3 ambient = sourceStrength * atmosphereColor;
  vec3 sunDir = normalize(sunRay*radius*scale*2.5 - vcPos);
  float diff = max(dot(normalize(normal), sunDir), 0.0);
  vec3 diffuse = diff * rayleighScattering;


  vec3 result = (ambient + diffuse) * mieScattering;

  fColor.rgb = result;
  fColor.a   = 0.5f;
  fColor.a  *= (fColor.b);
  if(size!=0)
    fColor.a /= size;
  fColor.a /= 2.6667f;

    fColor.rgb = vec3(0.3f, 0.4f, .8f);

}


