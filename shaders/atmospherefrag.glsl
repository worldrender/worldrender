#version 430 core

uniform bool wireframe;

uniform float radius;
uniform float scale;
uniform bool io;
uniform int size;
uniform float near;
uniform float far;

uniform vec3 viewPos;
uniform vec3 atmosphereColor = vec3(0.1f, 0.1f, 0.2f);
uniform vec3 mieScattering = vec3(0.5f, 0.6f, .9f);
uniform vec3 rayleighScattering = vec3(0.4f, 0.5f, .9f);
uniform vec3 sunRay = vec3(-1, -0.3, 1);
uniform float time;

in vec3 vcNormal;
in vec3 vcPos;
in float w;

out vec4 fColor;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
float noise(float x) { float i = floor(x); float f = fract(x); float u = f * f * (3.0 - 2.0 * f); return mix(hash(i), hash(i + 1.0), u); }
float noise(vec2 x) { vec2 i = floor(x); vec2 f = fract(x); float a = hash(i); float b = hash(i + vec2(1.0, 0.0)); float c = hash(i + vec2(0.0, 1.0)); float d = hash(i + vec2(1.0, 1.0)); vec2 u = f * f * (3.0 - 2.0 * f); return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y; }
float noise(vec3 x) { const vec3 step = vec3(110, 241, 171); vec3 i = floor(x); vec3 f = fract(x); float n = dot(i, step); vec3 u = f * f * (3.0 - 2.0 * f); return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x), mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y), mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x), mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z); }

#define DEFINE_FBM(name, OCTAVES) float name(vec3 x) { float v = 0.0; float a = 0.5; vec3 shift = vec3(100); for (int i = 0; i < OCTAVES; ++i) { v += a * noise(x); x = x * 2.0 + shift; a *= 0.5; } return v; }
DEFINE_FBM(fbm3, 3)
DEFINE_FBM(fbm5, 5)

float hash2(float n) { return fract(sin(n) * 1e4); }
float hash2(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noised(vec3 x) {
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);

	// For performance, compute the base input to a 1D hash from the integer part of the argument and the
	// incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash2(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash2(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash2(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash2(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );


float fbm( in vec3 p, int octaves, float gain, float amplitude, float frequency, float size){
    float f = 0.0;
    for(int i=0;i<octaves;i++)
    {
      f+=noised(p)*frequency;
      p = m3*p*(2.f+i/100.f);
      frequency /= 2.f;
    }

    f *= size;

    return f/amplitude;
}


float fAbs(float t)
{
  return t>0 ? t : -t;
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {

  vec3 normal = normalize(vcNormal);
  normal = normalize(normal);

  if(dot(vcPos-viewPos,normal)<=0)
    discard;

  float sourceStrength = 1;
  vec3 ambient = sourceStrength * atmosphereColor;
  vec3 sunDir = normalize(sunRay*radius*scale*2.5 - vcPos);
  float diff = max(dot(normalize(normal), sunDir), 0.0);
  vec3 diffuse = diff * rayleighScattering;


  vec3 result = (ambient + diffuse) * mieScattering;
  fColor.rgb = vec3(0);
  fColor.a   = 1;
  if(size==1||size==0)
  {
    float len = length(vcPos);
    float s = sin(len)*0.1;
    float c = cos(len)*0.1;
    float s2 = 2.0*s*c;
    float c2 = 1.0-2.0*s*s;
    float s3 = s2*c + c2*s;

    vec3 offset   = vec3(6.0*sin(cos(len*1.1)), 3.0*cos(sin(len*1.1)),2*sin(cos(len*1.1)));
    vec3 newPos   = vec3(vcPos.x*c2-vcPos.y*s2,vcPos.y*c2+vcPos.x*s2,vcPos.z*c2*s2);
    newPos        = newPos*(1.0+0.2*s3) - offset;

    float beta = sin(newPos.y*2.0 + len*8.0);
    float betaz = cos(newPos.z*2.0 + len*8.0);
    newPos.x = newPos.x + 0.4*beta;
    newPos.y = newPos.y - 0.4*beta;
    newPos.z = newPos.z + 0.4*beta;
    //newPos = (vec4(newPos,1.f)*rotationMatrix(vec3(1.f,0.f,0.f),time)).xyz;
    newPos -= time*0.00001;

    vec3 pos = vcPos - time*0.00001;

    float sky = (fbm3(newPos* 0.015));
    float warp1 = fbm3(newPos*0.1+10)+0.1;

    float warp  = fbm3(newPos*0.1+(fbm3(newPos*0.1+fbm3(newPos*0.1+fbm3(newPos*0.1)))));
    float warp2 = fbm3(vcPos*0.1+(fbm3(vcPos*0.1+fbm3(vcPos*0.1+fbm3(vcPos*0.1)))));
    float warp3 = fbm3(pos*0.1+(fbm3(pos*0.1+fbm3(pos*0.1+fbm3(pos*0.1)))));
    float warpedx = fbm3(vec3(sin(vec3(newPos.x/25,newPos.y/30,newPos.z/7-tan(newPos.z))-0.55))+0.55);
    float warpedy = fbm3(vec3(cos(vec3(newPos.x/7,newPos.y/25,newPos.z/30-tan(newPos.z))+0.55))-0.55);
    sky -= warpedx*0.09;
    sky -= warpedy*0.09;
    sky += warp;
    sky -= warp1;
    sky -= warp2;
    sky += warp3;
    sky -= sin(fbm(newPos,16, 1.f, .93753125f, .5f, 1))*0.5;
    fColor.rgb = vec3(1)*diff;
    if(size!=0)
      fColor.a = sky*fColor.r;
    else
      fColor.rgb *= sky*fColor.r+0.4;

  }

    fColor.rgb += result;
    fColor.a   /= 2.f;
    fColor.a  *= (fColor.b);
    if(size!=0&&size!=1)
    {
      fColor.a /= size;
      //fColor.a /= 2.6667f;

    }
          if(!io)
      {
        fColor.a += 0.3;
      }
    float b = fColor.b;
  //  fColor.rgb = vec3(0.3f, 0.4f, .8f);
    fColor.rgb = mix(vec3(0.3f, 0.4f, .8f),vec3(.9f,0.3f,0.2f),1-b*3)-fColor.rgb;
    fColor.rgb = mix(fColor.rgb,vec3(0.9,0.9,1),b);
    fColor.rgb = mix(fColor.rgb,vec3(0.3f, 0.4f, .8f)*2,b/3)+vec3(0,0,1)*b;
    fColor.a *= 0.95f;
}

