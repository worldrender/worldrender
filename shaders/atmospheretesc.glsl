#version 430 core

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];

out vec3 tcPosition[];
out vec3 tcNormal[];

#define ID gl_InvocationID


void main(){
  float TessLevelInner = 6;
  float e0, e1, e2;

  e0 = e1 = e2 = TessLevelInner;
  // tcTexCoord[ID]  = TexCoord[ID];
  tcPosition[ID]  = vPosition[ID];
  tcNormal[ID]    = vNormal[ID];

  if (ID == 0) {
    gl_TessLevelInner[0] = TessLevelInner;
    gl_TessLevelOuter[0] = e0;
    gl_TessLevelOuter[1] = e1;
    gl_TessLevelOuter[2] = e2;
  }

}

