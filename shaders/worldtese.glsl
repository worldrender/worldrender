#version 430 core
#define F3 0.333333333
#define G3 0.166666667
#define M_PI 3.14159265358979323844f
#define WIDTH 1280u
#define GRANULARITY 5.f


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
out float fNoise;
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

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float snoise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float sNoise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}


float ridgedNoise(in vec3 p, int octaves = 11, float H=0.7, float gain=0.03f, float amplitude = 1.f, float frequency = 0.03f, float persistence = 0.5f, float offset=0.01f)
{
  float total = 0.f;
  float exponent = pow(amplitude, -H);
  for(int i=0;i<octaves;i++)
  {
    total += offset-(((1.0 - abs(noise(p * frequency))) * 2.0 - 1.0) * amplitude*exponent);
    frequency	*= 1.f;
    amplitude *= gain;
  }
  return total;
}

float cubeVal(in float p)
{
  return p*p*p;
}

vec3 cubeVal(in vec3 p)
{
  return p*p*p;
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );

float fbm( in vec3 p, int octaves=16, float gain=1, float amplitude=0.93753125f, float frequency=1.f, float size=1){
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
    float vertexNoise = gl_TessCoord.x*tNoise[0]+gl_TessCoord.y*tNoise[1]+gl_TessCoord.z*tNoise[2];

    //tePosition*= radius;
    //tePosition.y = iqfBm(tePosition, 1,2,0.5);

    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];
    vcNormal = normalize(n0 + n1 + n2);
//    vNoise = vNoise*clamp(cubeNoise(vcPos),0,1);
    vNoise = (fbm(vcPos*10f))*0.4f;
    vNoise *= sin(cubeVal(vNoise));
    float mountains = (ridgedNoise(vcPos));

//     float h1 = hyrbidMultifractal(p/8.0, H, lacunarity, octaves, offset, gain);
//    float h2 = hyrbidMultifractal(p/3.0, H, lacunarity, octaves, offset, gain/2.0)*2.0;
//    float h3 = hyrbidMultifractal(p*2.0, H, lacunarity, octaves, offset, gain)*0.3;

    float f1 = ridgedNoise(vcPos/8.f, 5, 0.7, 0.7f, 4.f, 0.03f, 0.5f, 0.05f);
    float f2 = ridgedNoise(vcPos/3.f, 5, 0.7, 0.4f, 4.f, 0.03f, 0.5f, 0.05f)*2.f;
    float f3 = ridgedNoise(vcPos/2.f, 5, 0.7, 0.4f, 4.f, 0.03f, 0.5f, 0.05f)*0.3f;


    vNoise += cubeVal(mountains)*clamp((f1),-(GRANULARITY),GRANULARITY)+clamp((f2),-(GRANULARITY)/3,GRANULARITY/3)+clamp((f3),-(GRANULARITY),GRANULARITY)-0.8;
    vNoise += (vertexNoise);
    vNoise *= 1.1;
//    float signal = 1;
//    if(vNoise<0)
//      signal = -1;
//      vNoise += signal*(sin(vNoise));
    vcPos = vcPos + vcNormal * (vNoise);
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

