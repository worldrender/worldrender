#version 430 core

uniform bool wireframe;

uniform float radius;
uniform float scale;
uniform bool io;
uniform int size;

uniform vec3 viewPos;
uniform vec3 atmosphereColor = vec3(0.1f, 0.1f, 0.2f);
uniform vec3 mieScattering = vec3(0.5f, 0.6f, .9f);
uniform vec3 rayleighScattering = vec3(0.4f, 0.5f, .9f);
uniform vec3 sunRay = vec3(-1, -0.3, 1);

in vec3 vcNormal;
in vec3 vcPos;

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
  if(size==10)
  {
    float sky = (fbm3(vcPos * 0.015));
    float warpedx = fbm3(vec3(sin(vec3(vcPos.x/25,vcPos.y/30,vcPos.z/7)-0.55))+0.55);
    float warpedy = fbm3(vec3(cos(vec3(vcPos.x/7,vcPos.y/25,vcPos.z/30)+0.55))-0.55);
    sky -= warpedx*0.05;
    sky -= warpedy*0.05;
    sky -= sin(fbm(vcPos,16, 1.f, .93753125f, .5f, 1))*0.5;
    fColor.rgb = vec3(1)*diff*1.2;

    fColor.a = sky*fColor.r+0.2;

  }
  else
  {
    fColor.rgb = result;
    fColor.a   = 0.5f;
    fColor.a  *= (fColor.b);
    if(size!=0)
      fColor.a /= size;
    fColor.a /= 2.6667f;
    if(!io)
      fColor.a += 0.3;
    float b = fColor.b;
  //  fColor.rgb = vec3(0.3f, 0.4f, .8f);
    fColor.rgb = mix(vec3(0.3f, 0.4f, .8f),vec3(.9f,0.3f,0.2f),1-b*3)-fColor.rgb;
    fColor.rgb = mix(fColor.rgb,vec3(0.9,0.9,1),b);
    fColor.rgb = mix(fColor.rgb,vec3(0.3f, 0.4f, .8f)*2,b/3)+vec3(0,0,1)*b;
    fColor.a *= 0.95f;
  }
}

