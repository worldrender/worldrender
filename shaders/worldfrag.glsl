#version 430 compatibility
#define M_PI 3.14159265358979323844f
#define M_E 2.718281828459045235360f
#define SC 250.f
#define RADIUS 200.f
#define max_height 20.f

// materials
	#define c_water vec3(.015, .110, .455)
	#define c_grass vec3(.086, .132, .018)*0.8f
	#define c_beach vec3(.153, .172, .121)*0.6f
	#define c_rock  vec3(.080, .050, .030)*0.8f
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
in float fNoise;
in vec3 tNormal;

uniform sampler2D pTexture;
uniform sampler2D dTexture;
uniform sampler2D nTexture;


uniform sampler2D grasText;
uniform sampler2D soilText;
uniform sampler2D snowText;
uniform sampler2D maskText;

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

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and
    // V, the view vector (vertex to eye)
   vec3 map = texture(nTexture, texcoord ).xyz;
   map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
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

float saturateFloat(float source)
{
  return clamp(source, 0.0, 1.0);
}

void main() {

  vec3 normal = normalize(vcNormal);
//  if(dot(vcPos-viewPos,normal)>=0)
//    discard;

  if(!wireframe){
	fColor = vec4(0.f, 0.f, 0.f, 1.f);
 	return;
  }
  float hNoise = (vNoise)/2;

  vec3 fragPos = vec3(model*vec4(vcPos,1.0f));

  vec2 uv;
  const float kOneOverPi = 1.0 / M_PI;

  uv.s = 0.5 - 0.5 * atan( normal.x, -normal.z ) * kOneOverPi;
  uv.t = 1.0 - acos( normal.y ) * kOneOverPi;

  vec3 PN = perturb_normal(normal, vcPos, uv*2);

  float hL;
  float hR;
  float hD;
  float hU;

  vec3 col;
//  vec3 w_normal = calcNormal(normal,dot(normal,PN));
  vec3 c_normal = sdf_terrain_normal(vcPos);
  vec3 w_normal = normalize(cross(normal,PN));

  hU = length(normal);
  hL = dot(c_normal, normal);
  hR = hNoise/length(normal);
  hD = hNoise / hU;
  float hN = (hU-hL)/hNoise;
	float s = smoothstep(.4, 1., hN);

  vec3 Tangent;

  vec3 c1 = cross(w_normal, vec3(0.0, 0.0, 1.0));
  vec3 c2 = cross(w_normal, vec3(0.0, 1.0, 0.0));
  vec3 c3 = cross(w_normal, vec3(1.0, 0.0, 0.0));
  vec3 c4 = cross(w_normal, vec3(1.0, 1.0, 0.0));
  vec3 c5 = cross(w_normal, vec3(1.0, 0.0, 1.0));
  vec3 c6 = cross(w_normal, vec3(0.0, 1.0, 1.0));
  vec3 c7 = cross(w_normal, vec3(1.0, 1.0, 1.0));
  if (length(c1) > length(c2))
    Tangent = c1;
  else if(length(c2) > length(c3))
    Tangent = c2;
  else if(length(c3) > length(c4))
    Tangent = c3;
  else if(length(c4) > length(c5))
    Tangent = c4;
  else if(length(c5) > length(c6))
    Tangent = c5;
  else if(length(c6) > length(c7))
    Tangent = c6;
  else
    Tangent = c7;

	float angleDiff = abs(dot(normal, Tangent));
  float pureRock = 0.6;
  float lerpRock = 0.7;
  float coef = 1.0 - smoothstep(pureRock, lerpRock, angleDiff);

  float d = dot(normalize(normal), lightDir*vNoise);
  float cNoise = hNoise*d/8;

  vec3 snowAmbient = vec3(0.05f);
  vec3 snowSpecular = vec3(1.f);

  vec3 snowColor  = texture( soilText, uv*scale ).rgb*.15f;



//  float snowShininess
//
//  vec3 grassAmbient
//  vec3 grassDiffuse
//  vec3 grassSpecular
//  float grassShininess
//
//  vec3 soilAmbient
//  vec3 soilDiffuse
//  vec3 soilSpecular
//  float soilShininess

  vec3 grassColor = texture( grasText, uv*scale*radius ).rgb*.04f;
  vec3 soilColor  = texture( snowText, uv*scale*2 ).rgb*.045f;
  vec3 maskColor  = texture( maskText, 0.75+uv*scale*radius ).rgb*.055f;
  //snowColor = mix(snowColor,maskColor,hNoise+maskColor.g);
	vec3 rock = mix(
		snowColor+c_rock*hNoise/2, snowColor/2+c_snow*hNoise*0.3f,
		smoothstep(1. - .3*s-cNoise*0.5f, 1. - .2*s, hR/2));
	vec3 grass = mix(
		grassColor+c_grass, soilColor+rock*hNoise*0.3f,
		smoothstep(l_grass-cNoise, l_rock+cNoise, hNoise));

  vec3 deviant = mix(
		snowColor+c_rock*hNoise/2, snowColor/2+c_snow*hNoise*0.3f,
		smoothstep(1. - .3*s, 1. - .2*coef, coef));


  coef = smoothstep(0.90, 0.98, angleDiff);

  grass = mix(grass, deviant, coef*0.5f);
	vec3 shoreline = mix(
		soilColor+c_beach, grass,
		smoothstep(l_shore, l_grass, hNoise));

	vec3 water = mix(
		c_water / 2., c_water,
		smoothstep(0., l_water, hNoise));

	vec3 L = mat3(1.f) * normalize(lightDir);
	shoreline *= setup_lights(L, c_normal);
	vec3 ocean = setup_lights(L, normal) * water;

  col = mix(ocean, soilColor+shoreline,	smoothstep(l_water, l_shore, hNoise));
  col *= 1;
  fColor = vec4(col,1.f);

    fColor -= hNoise/20;

  vec3 ref = reflect( viewPos, c_normal );
  float fre = clamp( 1.0+dot(viewPos,c_normal), 0.0, 1.0 );
  vec3 hal = normalize(vNoise-lightDir-viewPos);
//
  col = (vNoise*0.25+0.75)*0.9*mix( vec3(0.10,0.05,0.03), vec3(0.13,0.10,0.08), clamp(fNoise/200.0,0.0,1.0) );
		col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*hNoise),smoothstep(0.70,0.9,hNoise-PN.y) );
        col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*hNoise),smoothstep(0.95,1.0,hNoise-PN.y) );

  float h = smoothstep(55.0,80.0,vcPos.y/SC + 25.0*hNoise );
  float e = smoothstep(1.0-0.5*h,1.0-0.1*h,hNoise-PN.y);
  float o = 0.3 + 0.7*smoothstep(0.0,0.1,hNoise-PN.x+h*h);
  float u = 0.2 + 0.2*smoothstep(0.5*h,0.1-0.2*h,hNoise-PN.z+h*h);
        s = u*h*e*o;
  col = mix( col, 0.29*vec3(0.62,0.65,0.7), smoothstep( 0.1, 0.9, s ) );
  col *= 1;

  float amb = clamp(0.5+0.5*dot(ambient,normalize(PN*hNoise)),0.0,1.0);
  float dif = clamp( dot( diffuse, normalize(PN*hNoise) ), 0.0, 1.0 );
  float bac = clamp( 0.2 + 0.8*dot( normalize( vec3(-diffuse.x, 0.0, diffuse.z ) ), PN ), 0.0, 1.0 );
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
  fColor = vec4(mix(col,fColor.rgb,0.3666f),1);

  vec3 textDiff = smoothstep(c_snow * fColor.rgb,snowColor,saturate(vec4(angleDiff)).rgb);
  col = fColor.rgb * textDiff;
  col *= fColor.rgb;
  fColor.rgb -= col;

  //fColor *= vec4(lin,1);
  fColor *= 1.77773-(hNoise>0?fNoise:0);

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
  vec3 reflection = -reflect(lightVec, normalize(vec3(tan(PN.x),tan(PN.y),tan(PN.z))));

  float shine = dot(normalize(reflection), normalize(vec3(MVP[0][2],MVP[1][2],MVP[2][2])));

  float specularShininess = 10.0f;

  if(shine > 0.0f)
  {
    shine = pow(shine, specularShininess);
    final = saturate(final+shine);
  }

  fColor = mix(fColor, final/2, fColor.b);

  fColor = vec4(abs(sin(pow(M_E,fColor.r)))*fColor.r,abs(sin(pow(M_E,fColor.g)))*fColor.g,abs(sin(pow(M_E,fColor.b)))*fColor.b,1.f);

  //fColor = vec4(hNoise,hNoise,hNoise,1.f);

  fColor.rgb *= bac*bac;
  fColor.rgb *= 2.3f;


}


