#version 430 core
#define F3 0.333333333
#define G3 0.166666667
#define M_PI 3.14159265358979323844f

layout(triangles, equal_spacing, ccw) in;

uniform mat4 MVP;
uniform mat4 model;
uniform float radius;
uniform float scale;
uniform bool io;

in vec3 tcPosition[];
in vec3 tcNormal[];
in float tNoise[];

out vec3 vcNormal;
out vec3 vcPos;

void main(){
    vec3 tcPos0 = (tcPosition[0]);
    vec3 tcPos1 = (tcPosition[1]);
    vec3 tcPos2 = (tcPosition[2]);

    vec3 p0 = gl_TessCoord.x * tcPos0;
    vec3 p1 = gl_TessCoord.y * tcPos1;
    vec3 p2 = gl_TessCoord.z * tcPos2;
    vcPos = (p0 + p1 + p2);

    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];

    vec3 sphereCoord = normalize(vcPos);
    vcPos = mix(vcPos, sphereCoord*radius, 1)*(scale);
    vcPos /= 1.03;
    vcPos = vec3(model * vec4(vcPos, 1.0));
    vcNormal = normalize(vcPos);
    gl_Position = MVP * vec4(vcPos, 1.0);
}

