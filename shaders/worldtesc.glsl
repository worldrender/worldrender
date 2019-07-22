#version 430 core
#define TESS_LEVEL 2

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
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

#define ID gl_InvocationID

//float LOD(vec3 posV, vec3 cam){
//  float dist = distance(posV, cam);
//  if(dist<=radius*1.05*scale/5) return 32.0;
//  else if(dist>radius*1.05*scale/5 && dist<=radius*1.05*scale/4) return 16.0;
//  else if(dist>radius*1.05*scale/4 && dist<=radius*1.05*scale/3) return 8.0;
//  else if(dist>radius*1.05*scale/3 && dist<=radius*1.05*scale/2) return 4.0;
//  else if(dist>radius*1.05*scale/2 && dist<=radius*1.05*scale) return 2.0;
//  else if(dist>radius*1.05*scale) return 1.0;
//}

float dlodCameraDistance(vec3 p0, vec3 p1)
{
    vec4 p40 = V * vec4(p0,1);
    vec4 p41 = V * vec4(p1,1);
	float MinDepth = radius;
	float MaxDepth = radius*scale*1.5;

	float d0 = clamp( abs(p40.z - MinDepth)/ (MaxDepth - MinDepth), 0.0, 1.0);
	float d1 = clamp( abs(p41.z - MinDepth)/ (MaxDepth - MinDepth), 0.0, 1.0);

	float t = mix(16, 2, (d0 + d1) * 0.5);

	if (t <= 2.0)
	{
		return 2.0;
	}
	if (t <= 4.0)
	{
		return 4.0;
	}
	if (t <= 8.0)
	{
		return 8.0;
	}
	return 16.0;

}

void main(){

    // tcTexCoord[ID]  = TexCoord[ID];
    tcPosition[ID]  = vPosition[ID];
    tcNormal[ID]    = vNormal[ID];
    tcColor[ID]     = vColor[ID];
    tNoise[ID]      = vertNoise[ID];

    float e0, e1, e2;
    vec3 d0, d1, d2;

    d0 = gl_in[1].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz)/2;
    d1 = gl_in[0].gl_Position.xyz+(gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;
    d2 = gl_in[0].gl_Position.xyz+(gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz)/2;

    e0 = dlodCameraDistance(d2, d0);
    e1 = dlodCameraDistance(d0, d1);
    e2 = dlodCameraDistance(d1, d2);

    float m = min(e0,min(e1,e2));

    gl_TessLevelOuter[0] = e0;
    gl_TessLevelOuter[1] = e1;
    gl_TessLevelOuter[2] = e2;
    gl_TessLevelInner[0] = floor((min(e0,min(e1,e2))+max(e0,max(e1,e2)))/2);

}

