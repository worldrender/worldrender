#version 430 core

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec4 vColor[];
//in vec2 TexCoord[];

//out vec2 tcTexCoord[];
out vec3 tcPosition[];
out vec3 tcNormal[];
out vec4 tcColor[];

uniform float px;
uniform float py;
uniform float pz;
uniform bool tess;

uniform float dx;
uniform float dy;
uniform float dz;
uniform float radius;

#define ID gl_InvocationID

float LOD(vec3 posV, float posCX, float posCY, float posCZ){
  float dist = sqrt((posV.x - posCX)*(posV.x - posCX)
                  + (posV.y - posCY)*(posV.y - posCY)
                  + (posV.z - posCZ)*(posV.z - posCZ));
  if(dist<=5) return 32.0;
  else if(dist>5 && dist<=15) return 16.0;
  else if(dist>15 && dist<=20) return 8.0;
  else if(dist>20 && dist<=30) return 4.0;
  else if(dist>30 && dist<=40) return 2.0;
  else if(dist>40) return 1.0;
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
  float TessLevelInner = 0;
  float TessLevelOuter = 0;
  // tcTexCoord[ID]  = TexCoord[ID];
  tcPosition[ID]  = vPosition[ID];
  tcNormal[ID]    = vNormal[ID];
  tcColor[ID]     = vColor[ID];


  if (ID == 0) {
    vec3 vPos = vPosition[0];
    if(tess){
      TessLevelInner = dirLOD(vPos, px, py, pz);
    }
    else{
      TessLevelInner = 1;
    }
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


