#version 430 core
#define M_PI 3.14159265358979323844f
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
in float vertNoise;
in float fNoise;

uniform sampler2D pTexture;
uniform sampler2D dTexture;
uniform sampler2D nTexture;

uniform mat4 model;
uniform vec3 viewPos;
uniform float time;
uniform float radius;

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
  float hNoise = (vNoise)/1.3;
  vec3 normal = normalize(vcNormal);
  vec3 fragPos = vec3(model*vec4(vcPos,1.0f));

  float hL;
  float hR;
  float hD;
  float hU;

 vec3 col;
  vec3 w_normal = sdf_terrain_normal(normal);
  hU = length(normal);
  hL = dot(w_normal, normal);
  hR = hNoise/length(normal);
  float Ar = hU - hNoise;
  hD = hNoise/hU ;
	float s = smoothstep(.4, 1., hNoise);
	vec3 rock = mix(
		c_rock, c_snow,
		smoothstep(1. - .3*s, 1. - .2*s, hNoise));

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
	shoreline *= setup_lights(L, w_normal);
	vec3 ocean = setup_lights(L, w_normal) * water;

  col = mix(ocean, shoreline,	smoothstep(l_water, l_shore, hNoise));
  col *= 0.888;
  fColor = vec4(col,1.f);

  vec3 ref = reflect( viewPos, w_normal );
  float fre = clamp( 1.0+dot(viewPos,w_normal), 0.0, 1.0 );
  vec3 hal = normalize(lightDir-viewPos);
//
  col = (hD*0.25+0.75)*0.9*mix( vec3(0.10,0.05,0.03), vec3(0.13,0.10,0.08), clamp(vertNoise/200.0,0.0,1.0) );
		col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*hD),smoothstep(0.70,0.9,hNoise-w_normal.y) );
        col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*hD),smoothstep(0.95,1.0,hNoise-w_normal.y) );

  float h = smoothstep(55.0,80.0,vcPos.y/SC + 25.0*hNoise );
  float e = smoothstep(1.0-0.5*h,1.0-0.1*h,hD-w_normal.y);
  float o = 0.3 + 0.7*smoothstep(0.0,0.1,hD-w_normal.x+h*h);
        s = h*e*o;
  col = mix( col, 0.29*vec3(0.62,0.65,0.7), smoothstep( 0.1, 0.9, s ) );
  col *= 1;

  float amb = clamp(0.5+0.5*dot(ambient,w_normal),0.0,1.0);
  float dif = clamp( dot( diffuse, w_normal ), 0.0, 1.0 );
  float bac = clamp( 0.2 + 0.8*dot( normalize( vec3(-diffuse.x, 0.0, diffuse.z ) ), w_normal ), 0.0, 1.0 );
  float sh = 1.0;
//
  vec3 lin  = vec3(0.0);
  lin += dif*vec3(7.00,5.00,3.00)*1.3*vec3( sh, sh*sh*0.5+0.5*sh, sh*sh*0.8+0.2*sh );
  lin += amb*vec3(0.40,0.60,1.00)*1.2;
      lin += bac*vec3(0.40,0.50,0.60);
  col += s*
       (0.04+0.96*pow(clamp(1.0+dot(hal,viewPos),0.0,1.0),5.0))*
       vec3(7.0,5.0,3.0)*dif*sh*
       pow( clamp(dot(normal,hal), 0.0, 1.0),16.0);

  fColor = vec4(mix(col,fColor.rgb,0.2666f),1);

  //fColor *= vec4(lin,1);
  fColor *= 1.77773;
}


