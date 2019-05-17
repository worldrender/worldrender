#version 430 core
#define M_PI 3.1415926f
#define SC 250.f
#define RADIUS 2*20*10*7*M_PI
#define max_height 20.f

// materials
	#define c_water vec3(.015, .110, .455)
	#define c_grass vec3(.086, .132, .018)
	#define c_beach vec3(.153, .172, .121)
	#define c_rock  vec3(.080, .050, .030)
	#define c_snow  vec3(.600, .600, .600)

	// limits
	#define l_water .05
	#define l_shore .17
	#define l_grass .211
	#define l_rock .351

in vec4 vcColor;
in vec3 vcNormal;
in vec3 vcPos;
in float vNoise;

uniform sampler2D pTexture;
uniform sampler2D dTexture;
uniform sampler2D nTexture;

uniform mat4 model;
uniform vec3 viewPos;
uniform float time;
uniform vec3 lightDir = vec3(-1, -0.3, 1);

uniform vec3 diffuse = vec3(1.0f, 0.5f, 0.2f);
uniform vec3 ambient = vec3(0.05f, 0.05f, 0.08f);

uniform float maxHeight = 10.7f;
uniform vec3 texOffset = vec3(1.0f/8192.0f, 1.0f/4096.0f, 0.0f);
mat2 m2 = mat2(1.6,-1.2,1.2,1.6);

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );

out vec4 fColor;


float fAbs(float t)
{
  return t>0 ? t : -t;
}


float lambertian(vec3 norm, vec3 lightDir)
{
  return max(dot(norm, -lightDir), 0);
}
vec3 dirLighting(vec3 dif, vec3 norm)
{
  vec3 diffuse = (dif) * lambertian(norm, lightDir);
  //vec3 specular = (spec * light.Color) * Blinn(norm, light.Direction, viewDir, specPow);
  return diffuse;// + specular;
}
float height(vec2 uv)
{
  return texture(pTexture, uv).r*maxHeight;
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(nTexture, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * maxHeight);
    return texCoords - p;
}


// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }
float hash( float n ){ return fract(sin(n)*43758.5453123);                     }
// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

vec3 noised( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);

    vec2 u = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0;

    float a = hash(n+  0.0);
    float b = hash(n+  1.0);
    float c = hash(n+ 57.0);
    float d = hash(n+ 58.0);

	return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,
				30.0*f*f*(f*(f-2.0)+1.0)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));

}

vec4 noised( in vec3 x )
{
    vec3 p = floor(x);
    vec3 w = fract(x);

    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    vec3 du = 30.0*w*w*(w*(w-2.0)+1.0);

    float n = p.x + 317.0*p.y + 157.0*p.z;

    float a = hash(n+0.0);
    float b = hash(n+1.0);
    float c = hash(n+317.0);
    float d = hash(n+318.0);
    float e = hash(n+157.0);
	  float f = hash(n+158.0);
    float g = hash(n+474.0);
    float h = hash(n+475.0);

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return vec4( -1.0+2.0*(k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z),
                      2.0* du * vec3( k1 + k4*u.y + k6*u.z + k7*u.y*u.z,
                                      k2 + k5*u.z + k4*u.x + k7*u.z*u.x,
                                      k3 + k6*u.x + k5*u.y + k7*u.x*u.y ) );
}

float terrain3 (in vec3 x, float octaves = 14)
{
  float f = 1.98;  // could be 2.0
  float s = 0.49;  // could be 0.5
  float a = 0.0;
  float b = 0.5;
  vec3  d = vec3(0.0);

  mat3  m = mat3( 1.0,0.0,0.0,
                0.0,1.0,0.0,
                0.0,0.0,1.0 );

  for( int i=0; i < octaves; i++ )
  {
  vec4 n = noised(x);
    a += b*n.x;          // accumulate values
    d += b*m*n.yzw;      // accumulate derivatives
    b *= s;
    x = f*m3*x;
    m = f*m3i*m;
  }
  return a;
}

float terrain2( in vec2 x )
{
	vec2  p = x*0.003;
    float a = 0.0;
    float b = 1.0;
	vec2  d = vec2(0.0);
  for(int i=0;i<14; i++)
  {
      vec3 n = noised(p);
      d += n.yz;
      a += b*n.x/(1.0+dot(d,d));
      b *= 0.5;
      p=m2*p;
  }

  return 140.0*a;
}

float map2( in vec3 p )
{
	float h = terrain2(p.xz);


	float ss = 0.03;
	float hh = h*ss;
	float fh = fract(hh);
	float ih = floor(hh);
	fh = mix( sqrt(fh), fh, smoothstep(50.0,140.0,h) );
	h = (ih+fh)/ss;

    return p.y - h;
}

vec3 calcNormal( in vec3 pos, float t )
{
	float e = 0.001;
	e = 0.001*t;
    vec3  eps = vec3(e,0.0,0.0);
    vec3 nor;
    nor.x = map2(pos+eps.xyy) - map2(pos-eps.xyy);
    nor.y = map2(pos+eps.yxy) - map2(pos-eps.yxy);
    nor.z = map2(pos+eps.yyx) - map2(pos-eps.yyx);
    return normalize(nor);
}

vec2 sdf_terrain_map_detail(in vec3 pos)
{
	float h0 = terrain3(pos * 2.0987);
	float n0 = smoothstep(.35, 1., h0);

	float h1 = terrain3(pos * 1.50987 + vec3(1.9489, 2.435, .5483));
	float n1 = smoothstep(.6, 1., h1);

	float n = n0 + n1;

	return vec2(length(pos) - RADIUS - n * max_height, n / max_height);
}

vec3 sdf_terrain_normal(in vec3 p)
{
#define F(t) sdf_terrain_map_detail(t).x
	vec3 dt = vec3(0.001, 0, 0);

	return normalize(vec3(
		F(p + dt.xzz) - F(p - dt.xzz),
		F(p + dt.zxz) - F(p - dt.zxz),
		F(p + dt.zzx) - F(p - dt.zzx)
	));
#undef F
}

vec3 setup_lights(
	in vec3 L,
	in vec3 normal
){
	vec3 diff = diffuse;

	// key light
	vec3 c_L = vec3(7, 5, 3);
	diff += max(0., dot(L, normal)) * c_L;

	// fill light 1 - faked hemisphere
	float hemi = clamp(.25 + .5 * normal.y, .0, 1.);
	diff += hemi * vec3(.4, .6, .8) * .2;

	// fill light 2 - ambient (reversed key)
	float amb = clamp(.12 + .8 * max(0., dot(-L, normal)), 0., 1.);
	diff += amb * ambient * vec3(.4, .5, .6);

	return diff;
}



void main() {
  float hNoise = (vNoise);
  vec3 normal = normalize(vcNormal);

  vec3 fragPos = vec3(model*vec4(vcPos,1.0f));

  vec2 uv = vec2((atan(normal.y, normal.x) / M_PI + 1.0) * 0.5,
                                  (asin(normal.z) / M_PI + 0.5));
  //uv = vec2(atan( normal.y, normal.x )/2*M_PI, asin( normal.z )/M_PI);

  vec3 detail = texture(dTexture, uv*hNoise).rgb/hNoise;

  float hL = height(uv - texOffset.xz);
  float hR = height(uv + texOffset.xz);
  float hD = height(uv - texOffset.zy);
  float hU = height(uv + texOffset.zy);
  vec3 N = normalize(vec3((hL - hR)/(hNoise), (hD - hU)/(hNoise), hNoise));

  vec3 norm = normalize(normal);
  vec3 up = vec3(0, 1, 0)-norm;
  vec3 tang = normalize(cross(norm, up));//might need flipping
  vec3 biTan = normalize(cross(norm, tang));//same
  mat3 localAxis = mat3(tang, biTan, norm);
  norm = normalize(localAxis * normalize(N));

  mat3 TBN = transpose(mat3(tang, biTan, norm));
  vec3 viewDir = normalize((viewPos*TBN)-(fragPos*TBN));
  uv = ParallaxMapping(uv,  viewDir)*hNoise*0.3333f;
//  if(uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
//        discard;
  vec3 parallax = texture(nTexture,uv).rgb/hNoise*0.2222f;
  vec3 light = dirLighting(detail, norm);
  light*=0.8f;
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

  fColor = vec4(light+ambient*detail, 1.0f);

  float diffP =  max(dot(light, parallax), 0.0);
        diffP += max(dot(light, detail), 0.0)*0.233333f;
  vec3 diffuse = diffP*ambient;

  vec3 reflectDir =  reflect(-light, parallax);
       reflectDir += reflect(-light, detail)*0.233333f;
  vec3 halfwayDir = normalize(light + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  vec3 specular = vec3(0.2) * spec;

  vec4 difSpe = vec4(diffuse+spec,1.0f);
  float lColor = (0.08333f+1/length(viewPos));
  fColor = mix(fColor,difSpe,0.222);
  vec4 sColor = smoothstep(fColor,difSpe,vec4(0.4));
  fColor = mix(fColor,sColor,vec4(0.222222));

//  vec4 noise = vec4(vec3(random(fColor.xyz)),1.f);
//
//  fColor = mix(fColor,noise,0.09333f);
  vec4 noise = vec4(random(vcPos.x),random(vcPos.y),random(vcPos.z),1.f);
  fColor = mix(fColor,noise,lColor);
  fColor *= 0.333f;
  noise = fColor;
  vec3 col;
  vec3 w_normal = sdf_terrain_normal(normal);
  hU = length(normal);
  hL = dot(w_normal, normal);
  hR = hNoise/length(normal);
  float Ar = hU - hNoise;
  hD = hNoise-hL ;
  float s = smoothstep(.4, 1., hD);
	vec3 rock = mix(
		c_rock, c_snow,
		smoothstep(1. - .3*s, 1. - .2*s, hL));

	vec3 grass = mix(
		c_grass, rock,
		smoothstep(l_grass, l_rock, hD));

	vec3 shoreline = mix(
		c_beach, grass,
		smoothstep(l_shore, l_grass, hD));

	vec3 water = mix(
		c_water / 2., c_water,
		smoothstep(0., l_water, hD));

	vec3 L = mat3(1.f) * normalize(vec3(1, 1, 0));
	shoreline *= setup_lights(L, normal);
	vec3 ocean = setup_lights(L, w_normal) * water;

  col = mix(ocean, shoreline,	smoothstep(l_water, l_shore, hR));
  col *= 0.5555f;
  fColor = vec4(mix(col,fColor.rgb,0.333333*smoothstep(l_water, l_shore, hR)),1);

  vec3 ref = reflect( viewPos, normal );
  float fre = clamp( 1.0+dot(viewPos,normal), 0.0, 1.0 );
  vec3 hal = normalize(lightDir-viewPos);

  col = (hD*0.25+0.75)*0.9*mix( vec3(0.10,0.05,0.03), vec3(0.13,0.10,0.08), clamp(terrain2( vec2(vcPos.x,vcPos.y*48.0))/200.0,0.0,1.0) );
		col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*hD),smoothstep(0.70,0.9,hD-normal.y) );
        col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*hD),smoothstep(0.95,1.0,hD-normal.y) );

  float h = smoothstep(55.0,80.0,vcPos.y/SC + 25.0*terrain2(0.01*vcPos.xz/SC) );
  float e = smoothstep(1.0-0.5*h,1.0-0.1*h,hD-normal.y);
  float o = 0.3 + 0.7*smoothstep(0.0,0.1,hD-normal.x+h*h);
        s = h*e*o;
  col = mix( col, 0.29*vec3(0.62,0.65,0.7), smoothstep( 0.1, 0.9, s ) );
  col *= 2;

  float amb = clamp(0.5+0.5*dot(ambient,normal),0.0,1.0);
  float dif = clamp( dot( diffuse, normal ), 0.0, 1.0 );
  float bac = clamp( 0.2 + 0.8*dot( normalize( vec3(-diffuse.x, 0.0, diffuse.z ) ), normal ), 0.0, 1.0 );
  float sh = 1.0;

  vec3 lin  = vec3(0.0);
  lin += dif*vec3(7.00,5.00,3.00)*1.3*vec3( sh, sh*sh*0.5+0.5*sh, sh*sh*0.8+0.2*sh );
  lin += amb*vec3(0.40,0.60,1.00)*1.2;
      lin += bac*vec3(0.40,0.50,0.60);
  col += s*
       (0.04+0.96*pow(clamp(1.0+dot(hal,viewPos),0.0,1.0),5.0))*
       vec3(7.0,5.0,3.0)*dif*sh*
       pow( clamp(dot(normal,hal), 0.0, 1.0),16.0);

  fColor = vec4(mix(col,fColor.rgb,0.444),1);

  fColor *= vec4(lin,1);
  fColor *= 1.77773;
}


