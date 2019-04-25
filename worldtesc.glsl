#version 430 core

layout(vertices = 4) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec4 vColor[];
//in vec2 TexCoord[];

//out vec2 tcTexCoord[];
out vec3 tcPosition[];
out vec3 tcNormal[];
out vec4 tcColor[];

uniform vec3 viewPos;

uniform bool tess;

uniform float dx;
uniform float dy;
uniform float dz;
uniform float radius;

#define ID gl_InvocationID

float LOD(vec3 posV, vec3 cam){
  float dist = distance(posV, cam);
  if(dist<=25) return 32.0;
  else if(dist>25 && dist<=75) return 16.0;
  else if(dist>75 && dist<=100) return 8.0;
  else if(dist>100 && dist<=150) return 4.0;
  else if(dist>150 && dist<=200) return 2.0;
  else if(dist>200) return 1.0;
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
  float TessLevelOuter = 1;
  // tcTexCoord[ID]  = TexCoord[ID];
  tcPosition[ID]  = vPosition[ID];
  tcNormal[ID]    = vNormal[ID];
  tcColor[ID]     = vColor[ID];


  if (ID == 0) {
    vec3 bTriangulo = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz
                       + gl_in[2].gl_Position.xyz)/3;

    TessLevelInner = LOD(bTriangulo, viewPos);
    TessLevelOuter = TessLevelInner;//dirLOD(vPos, px, py, pz);

    gl_TessLevelInner[0] = TessLevelInner;
    gl_TessLevelOuter[0] = TessLevelOuter;
    gl_TessLevelOuter[1] = TessLevelOuter;
    gl_TessLevelOuter[2] = TessLevelOuter;
  }
  if(TessLevelInner == 8.0 || TessLevelOuter == 8.0){
    tcColor[ID] == vec4(1.0, 1.0, 1.0, 1.0);
  }

}

