#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform bool wireframe;

void main()
{
  if(!wireframe){
    discard;
  }

  else{
    FragColor = texture(skybox, TexCoords);
    discard;
  }
}
