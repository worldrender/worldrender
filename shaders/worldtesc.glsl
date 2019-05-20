#version 430 core

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

#define ID gl_InvocationID

float LOD(vec3 posV, vec3 cam){
  float dist = distance(posV, cam);
  if(dist<=75) return 32.0;
  else if(dist>75 && dist<=150) return 16.0;
  else if(dist>150 && dist<=200) return 8.0;
  else if(dist>200 && dist<=300) return 4.0;
  else if(dist>300 && dist<=400) return 2.0;
  else if(dist>400) return 1.0;
}

float dirLOD(vec3 posV, float posCX, float posCY, float posCZ){
  float param = radius*100/4;
  vec3 direction = vec3(posCX, posCY, posCZ);
  float normal = dot(direction, posV);
  if(normal > param) return 8.0;
  else if(normal >= -param && normal <= param) return 4.0;
  else if(normal < -param) return 1.0;
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

    gl_TessLevelInner[0] = TessLevelInner;
    gl_TessLevelOuter[0] = e0;
    gl_TessLevelOuter[1] = e1;
    gl_TessLevelOuter[2] = e2;
  }
  if(TessLevelInner == 8.0 /*|| TessLevelOuter == 8.0*/){
    tcColor[ID] == vec4(1.0, 1.0, 1.0, 1.0);
  }

}

