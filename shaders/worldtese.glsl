#version 430 core
#define F3 0.333333333
#define G3 0.166666667
#define M_PI 3.14159265358979323844f

layout(triangles, equal_spacing, ccw) in;

uniform mat4 P;
uniform mat4 V;
uniform mat4 MVP;
uniform int oct;
uniform float lac;
uniform float radius;

in vec3 tcPosition[];
in vec3 tcNormal[];
in vec4 tcColor[];
in float tNoise[];
//in vec2 tcTexCoord[];

out float p;
out vec3 vcNormal;
out vec4 vcColor;
out float vNoise;
//out vec2 vcTexCoord;

out vec3 vcPos;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec3 x) {
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);

  float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x), mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
             mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x), mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );

float fbm( in vec3 p, int octaves=16, float gain=1, float amplitude=0.93753125f, float frequency=1, float size=1){
    float f = 0.0;
    for(int i=0;i<octaves;i++)
    {
      f+=noise(p*amplitude)*frequency;
      p = m3*p*(2.f+i/100.f);
      frequency /= 2.f;
      amplitude *= gain;
    }

    f *= size;

    return f/amplitude;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main(){
    vec3 tcPos0 = (tcPosition[0]);// tcPos0.y = iqfBm(tcPos0, 3, 8, 8)*radius;
    vec3 tcPos1 = (tcPosition[1]);// tcPos1.y = iqfBm(tcPos1, 3, 8, 8)*radius;
    vec3 tcPos2 = (tcPosition[2]);// tcPos2.y = iqfBm(tcPos2, 3, 8, 8)*radius;

    vec3 p0 = gl_TessCoord.x * tcPos0;
    vec3 p1 = gl_TessCoord.y * tcPos1;
    vec3 p2 = gl_TessCoord.z * tcPos2;
    vcPos = (p0 + p1 + p2);
    vNoise = gl_TessCoord.x*tNoise[0]+gl_TessCoord.y*tNoise[1]+gl_TessCoord.z*tNoise[2];

    //tePosition*= radius;
    //tePosition.y = iqfBm(tePosition, 1,2,0.5);

    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];
    vcNormal = normalize(n0 + n1 + n2);
    vNoise = fbm(vcPos)+vNoise*10;
    vcPos = vcPos + vcNormal * vNoise;
    vcNormal = normalize(vcPos);
    ///PASSAR O DELTA antes depois da normal
    vec4 c0 = gl_TessCoord.x * tcColor[0];
    vec4 c1 = gl_TessCoord.y * tcColor[1];
    vec4 c2 = gl_TessCoord.z * tcColor[2];
    vcColor = (c0 + c1 + c2);

    /*vec2 t0 = gl_TessCoord.x * tcTexCoord[0];
    vec2 t1 = gl_TessCoord.y * tcTexCoord[1];
    vec2 t2 = gl_TessCoord.z * tcTexCoord[2];
    vcTexCoord = (t0 + t1 + t2);
*/
    p = vcPos.y;
    gl_Position = MVP * vec4(vcPos, 1.0);
}

