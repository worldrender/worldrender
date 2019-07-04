#version 430 core
#define M_PI 3.14159265358979323844f
#define M_E 2.718281828459045235360f
#define SC 250.f
#define RADIUS 200.f
#define max_height 20.f

// materials
	#define c_water vec3(.015, .110, .455)
	#define c_grass vec3(.086, .132, .018)
	#define c_beach vec3(.153, .172, .121)
	#define c_rock  vec3(.080, .050, .030)
	#define c_snow  vec3(.30, .40, .550)

	// limits
	#define l_water .4
	#define l_shore .5
	#define l_grass .9
	#define l_rock .9

in vec4 vcColor;
in vec3 vcNormal;
in vec3 vcPos;
in float vNoise;
//in float vertNoise;
in float fNoise;
in float mNoise;

uniform sampler2D pTexture;
uniform sampler2D dTexture;
uniform sampler2D nTexture;

uniform float scale;
uniform mat4 model;
uniform mat4 MVP;
uniform vec3 viewPos;
uniform bool wireframe;
uniform float radius;

uniform vec3 lightDir = vec3(-1, -0.3, 1);

uniform vec3 diffuse = vec3(1.0f, 0.5f, 0.2f);
uniform vec3 ambient = vec3(0.05f, 0.05f, 0.08f);

uniform float maxHeight = 10.7f;
uniform vec3 texOffset = vec3(1.0f/8192.0f, 1.0f/4096.0f, 0.0f);
mat2 m2 = mat2(1.6,-1.2,1.2,1.6);

out vec4 fColor;

float fAbs(float t)
{
  return t>0 ? t : -t;
}

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/

	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));

	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);

	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;

	 /* 2. find four surflets and store them in d */
	 vec4 w, d;

	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);

	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);

	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);

	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;

	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

/* const matrices for 3d rotation */
const mat3 rot1 = mat3(-0.37, 0.36, 0.85,-0.14,-0.93, 0.34,0.92, 0.01,0.4);
const mat3 rot2 = mat3(-0.55,-0.39, 0.74, 0.33,-0.91,-0.24,0.77, 0.12,0.63);
const mat3 rot3 = mat3(-0.71, 0.52,-0.47,-0.08,-0.72,-0.68,-0.7,-0.45,0.56);

/* directional artifacts can be reduced by rotating each octave */
float simplex3d_fractal(vec3 m) {
    return   0.5333333*simplex3d(m*rot1)
			+0.2666667*simplex3d(2.0*m*rot2)
			+0.1333333*simplex3d(4.0*m*rot3)
			+0.0666667*simplex3d(8.0*m);
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

float map2( in vec3 p )
{
	float h = vNoise;


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

vec3 calcTangent(in vec3 a, in vec3 b)
{
  float c0 = fAbs(a.x)+fAbs(a.y)+fAbs(a.z);
  float c1 = fAbs(a.x)+fAbs(a.x)+fAbs(a.y);
  float c2 = fAbs(a.x)+fAbs(a.x)+fAbs(a.z);
  float c3 = fAbs(a.y)+fAbs(a.y)+fAbs(a.x);
  float c4 = fAbs(a.y)+fAbs(a.y)+fAbs(a.z);
  float c5 = fAbs(a.z)+fAbs(a.z)+fAbs(a.x);
  float c6 = fAbs(a.z)+fAbs(a.z)+fAbs(a.y);

  float d0 = fAbs(b.x)+fAbs(b.y)+fAbs(b.z);
  float d1 = fAbs(b.x)+fAbs(b.x)+fAbs(b.y);
  float d2 = fAbs(b.x)+fAbs(b.x)+fAbs(b.z);
  float d3 = fAbs(b.y)+fAbs(b.y)+fAbs(b.x);
  float d4 = fAbs(b.y)+fAbs(b.y)+fAbs(b.z);
  float d5 = fAbs(b.z)+fAbs(b.z)+fAbs(b.x);
  float d6 = fAbs(b.z)+fAbs(b.z)+fAbs(b.y);

  float e = 0.001;

  float c = c0+c1+c2+c3+c4+c5+c6;
  float d = d0+d1+d2+d3+d4+d5+d6;

  c *= e;
  d *= e;

  float sinC = sin(c);
  float sinD = sin(d);

  return normalize((a+sinC)*c+(b+sinD)*d)/2;
}

vec2 sdf_terrain_map_detail(in vec3 pos)
{
	float h0 = (vNoise * 2.0987);
	float n0 = smoothstep(.35, 1., h0);

	float h1 = vNoise * 1.50987 + length(vec3(1.9489, 2.435, .5483));
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
	vec3 c_L = vec3(7.1111f, 5.2222f, 3.3333f);
	diff += max(0., dot(L, normal)) * c_L;

	// fill light 1 - faked hemisphere
	float hemi = clamp(.255555 + .55555 * normal.y, .0, 1.);
	diff += hemi * vec3(.4, .6, .8) * .2;

	// fill light 2 - ambient (reversed key)
	float amb = clamp(.12 + .8 * max(0., dot(-L, normal)), 0., 1.);
	diff += amb * ambient * vec3(.4, .5, .6);

	return diff;
}

vec3 GetWaterColorAt(vec3 sourceColor, float depth)
{
float rIndex = 7.0f;
float gIndex = 10.0f;
float bIndex = 70.0f;

float sandRed = sourceColor.r;
float sandGreen = sourceColor.g;
float sandBlue = sourceColor.b;

float lostRed = (depth / rIndex) * (1.0f/3.0f);
float lostGreen = (depth / gIndex) * (1.0f/3.0f);
float lostBlue = (depth / bIndex) * (1.0f/3.0f);

sandRed = sandRed - (sandRed * lostRed);
sandGreen = sandGreen - (sandGreen * lostGreen);
sandBlue = sandBlue - (sandBlue * lostBlue);

vec3 sandColor = vec3(max(0, sandRed), max(0, sandGreen), max(0, sandBlue));
return sandColor;
}

vec4 saturate(vec4 source)
{
  return clamp(source, 0.0, 1.0);
}

void main() {
  if(!wireframe){
	fColor = vec4(0.f, 0.f, 0.f, 1.f);
 	return;
  }
  float hNoise = (vNoise)/2;
  vec3 normal = normalize(vcNormal);
  vec3 fragPos = vec3(model*vec4(vcPos,1.0f));

  float hL;
  float hR;
  float hD;
  float hU;

  vec3 col;
  vec3 w_normal = normalize(normal*sin(-20));
  vec3 c_normal = normalize(calcTangent(normal, normal*sin(20)));

  hU = length(normal);
  hL = dot(w_normal, normal);
  hR = hNoise/length(normal);
  hD = hNoise / hU;
  float hN = (hU-hL)/hNoise;
	float s = smoothstep(.4, 1., hN);
	vec3 rock = mix(
		c_rock, c_snow,
		smoothstep(1. - .3*s, 1. - .2*s, hR*0.4));

	vec3 grass = mix(
		c_grass, rock,
		smoothstep(l_grass, l_rock, hNoise));

	vec3 shoreline = mix(
		c_beach, grass,
		smoothstep(l_shore, l_grass, hNoise));

	vec3 water = mix(
		c_water / 2., c_water,
		smoothstep(0., l_water, hNoise));

	vec3 L = mat3(1.f) * normalize(lightDir);
	shoreline *= setup_lights(L, c_normal);
	vec3 ocean = setup_lights(L, c_normal) * water;

  col = mix(ocean, shoreline,	smoothstep(l_water, l_shore, hNoise));
  col *= 1;
  fColor = vec4(col,1.f);

  float value;
  value = simplex3d_fractal(vcPos*8.0+8.0);
	value = 0.5 + 0.5*value;
	value *= smoothstep(0.0, 0.005, abs(0.6-vcPos.x));
	vec4 noised = vec4(vec3(value),1.0);
  fColor -= noised/10;

  value = simplex3d_fractal(normal*8.0+8.0);
	value = 0.5 + 0.5*value;
	value *= smoothstep(0.0, 0.005, abs(0.6-normal.x));
  vec4 map = mix(fColor, noised, smoothstep(-1.f,1.f, hNoise));
  fColor -= noised/20;

  vec3 grain;
  grain = random3(vcPos*8.0+8.0);
	grain = 0.5 + 0.5*grain;
	grain *= smoothstep(0.0, 0.005, abs(0.6-vcPos.x));

  vec3 T = normalize(vcPos*grain);
  vec3 B = normalize(-normal*grain);
  grain = normalize(cross((B*T)*vNoise,noised.rgb));
  grain = vec3(simplex3d_fractal(grain*vNoise+vNoise));
  grain *= 0.05;

  fColor -= vec4(grain,1);
  vec3 normalNoise = normalize(vec3(sin(vNoise/4)/8,cos(vNoise/3)/6,sin(vNoise/4)/8));
  normalNoise = normalize(normalNoise);
  fColor += sin(vNoise/4)/8;
  fColor.rgb -= reflect(cross(fColor.rgb,normalNoise),noised.rgb)/10;

  grain = random3(vec3(vNoise*8.0+8.0));
	grain = 0.5 + 0.5*grain;
	grain *= smoothstep(0.0, 0.005, abs(0.6-vcPos.x));
	grain = normalize(grain);
	fColor.rgb -= grain/5;

  vec3 ref = reflect( viewPos, c_normal );
  float fre = clamp( 1.0+dot(viewPos,c_normal), 0.0, 1.0 );
  vec3 hal = normalize(lightDir-viewPos);
//
  col = (vNoise*0.25+0.75)*0.9*mix( vec3(0.10,0.05,0.03), vec3(0.13,0.10,0.08), clamp(fNoise/200.0,0.0,1.0) );
		col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*hD),smoothstep(0.70,0.9,vNoise-c_normal.y) );
        col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*hD),smoothstep(0.95,1.0,vNoise-c_normal.y) );

  float h = smoothstep(55.0,80.0,vcPos.y/SC + 25.0*vNoise );
  float e = smoothstep(1.0-0.5*h,1.0-0.1*h,hD-c_normal.y);
  float o = 0.3 + 0.7*smoothstep(0.0,0.1,hD-c_normal.x+h*h);
        s = h*e*o;
  col = mix( col, 0.29*vec3(0.62,0.65,0.7), smoothstep( 0.1, 0.9, s ) );
  col *= 1;

  float amb = clamp(0.5+0.5*dot(ambient,normalize(c_normal*mNoise)),0.0,1.0);
  float dif = clamp( dot( diffuse, normalize(c_normal*mNoise) ), 0.0, 1.0 );
  float bac = clamp( 0.2 + 0.8*dot( normalize( vec3(-diffuse.x, 0.0, diffuse.z ) ), c_normal ), 0.0, 1.0 );
  float sh = 1.0;
//
  vec3 lin  = vec3(0.0);
  lin += dif*vec3(7.00,5.00,3.00)*1.3*vec3( sh, sh*sh*0.5+0.5*sh, sh*sh*0.8+0.2*sh );
  lin += amb*vec3(0.40,0.60,1.00)*1.2;
      lin += bac*vec3(0.40,0.50,0.60);
  col *= s*
       (0.04+0.96*pow(clamp(1.0+dot(hal,viewPos),0.0,1.0),5.0))*
       vec3(7.0,5.0,3.0)*dif*sh*
       pow( clamp(dot(c_normal,hal), 0.0, 1.0),16.0);
  col *= col;
  fColor = vec4(mix(col,fColor.rgb,0.2666f),1);

  //fColor *= vec4(lin,1);
  fColor *= 1.77773;
  float d = dot(normalize(normal), lightDir*vNoise);

  vec4 specular = vec4(1.0, 1.0, 1.0,  1.0);
  vec4 emissive = vec4(GetWaterColorAt(c_water, hNoise), 1.0);
  float emissive_contribution = 0.80f;
  float ambient_contribution  = 0.30f;
  float diffuse_contribution  = 0.30f;
  float specular_contribution = 0.30f;
  vec4 final = fColor * emissive  * emissive_contribution +
               vec4(ambient,1.f) * ambient_contribution  +
               vec4(diffuse,1.f) * diffuse_contribution * max(d, 0);
  vec3 toEye = normalize(vcPos - viewPos);
  vec3 lightVec = normalize(-lightDir*radius*scale);
  vec3 reflection = -reflect(lightVec, normal);

  float shine = dot(normalize(reflection), normalize(vec3(MVP[0][2],MVP[1][2],MVP[2][2])));

  float specularShininess = 10.0f;

  if(shine > 0.0f)
  {
    shine = pow(shine, specularShininess);
    final = saturate(final+shine);
  }

  fColor = mix(fColor, final/2, fColor.b);
  fColor = vec4(fColor.rgb,1.f);

//  float x = smoothstep(0.2,0.5,fColor.x) - smoothstep(0.5,0.8,fColor.x);
//  float y = smoothstep(0.2,0.5,fColor.y) - smoothstep(0.5,0.8,fColor.y);
//  float z = smoothstep(0.2,0.5,fColor.z) - smoothstep(0.5,0.8,fColor.z);
//  vec4 mixture = vec4(x,y,z,1);
//  fColor = mix(fColor, mixture/2, fColor.r);
//  fColor = mix(fColor, mixture/3, fColor.g);
//  fColor = mix(fColor, mixture/4, fColor.b);

//  float gauss2 = (fColor.b-fColor.g)/2;
//  gauss2 *= gauss2;
//  float gauss = pow(M_E,gauss2)/(fColor.r*sqrt(2*M_PI));
//  gauss /= M_PI*M_E;
//  noised = vec4(simplex3d_fractal(fColor.rgb * gauss));
//  noised = fColor * gauss * noised;
//  noised /= 10;
//  noised = mix(fColor,noised,1);

  fColor *= 1.3;
}


