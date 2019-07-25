#version 430 core
#define TESS_LEVEL 1
#define M_PI 3.14159265358979323844f

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec4 vColor[];
in float vertNoise[];
//in vec2 TexCoord[];

//out vec2 tcTexCoord[];
out vec3 tcPosition[];
out vec3 tcNormal[];
out vec4 tcColor[];
out float tNoise[];

uniform vec3 viewPos;

uniform int tess;

uniform float dx;
uniform float dy;
uniform float dz;
uniform float radius;
uniform float scale;

#define ID gl_InvocationID

float LOD(vec3 posV, vec3 cam){
  float dist = distance(posV, cam);
  float rsc  = radius*1.05f*scale/2;
  float a = dist/rsc;
  a = floor(4/a);
  return 1+a*a*a/3;
}

void main(){
  float TessLevelInner = 1;
  float e0, e1, e2;

  e0 = e1 = e2 = 1;
  // tcTexCoord[ID]  = TexCoord[ID];
  tcPosition[ID]  = vPosition[ID];
  tcNormal[ID]    = vNormal[ID];
  tcColor[ID]     = vColor[ID];
  tNoise[ID]      = vertNoise[ID];
  vec3 d1, d2, d3;

  if (ID == 0) {
    vec3 bTriangulo = (vPosition[0] + vPosition[1]
                     + vPosition[2])/3;

    if(tess==0){
      TessLevelInner = LOD(bTriangulo, viewPos);

      d1=vPosition[1]+(vPosition[2]-vPosition[1])/2;
      d2=vPosition[0]+(vPosition[2]-vPosition[0])/2;
      d3=vPosition[0]+(vPosition[1]-vPosition[0])/2;

      e0=LOD(d1,viewPos);
      e1=LOD(d2,viewPos);
      e2=LOD(d3,viewPos);
    }
    else if(tess == 1){
      TessLevelInner = 1;
    }
    else if(tess == 2){
      TessLevelInner = 2;
    }
    //TessLevelOuter = TessLevelInner;//dirLOD(vPos, px, py, pz);

    gl_TessLevelInner[0] = TessLevelInner*TESS_LEVEL;
    gl_TessLevelOuter[0] = e0*TESS_LEVEL;
    gl_TessLevelOuter[1] = e1*TESS_LEVEL;
    gl_TessLevelOuter[2] = e2*TESS_LEVEL;
  }
  if(TessLevelInner == 8.0 /*|| TessLevelOuter == 8.0*/){
    tcColor[ID] == vec4(1.0, 1.0, 1.0, 1.0);
  }

}

