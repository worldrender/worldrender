#version 430 core
#define M_PI 3.1415926f
in vec4 vcColor;
in vec3 vcNormal;
in vec3 vcPos;

uniform sampler2D pTexture;
uniform sampler2D dTexture;
uniform sampler2D nTexture;

uniform mat4 model;
uniform vec3 viewPos;
uniform vec3 lightDir = vec3(-1, -0.3, 1);

uniform vec3 diffuse = vec3(1.0f, 0.5f, 0.2f);
uniform vec3 ambient = vec3(0.05f, 0.05f, 0.08f);

uniform float maxHeight = 10.7f;
uniform vec3 texOffset = vec3(1.0f/8192.0f, 1.0f/4096.0f, 0.0f);


out vec4 fColor;

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

void main() {
  vec3 normal = normalize(vcNormal);
  vec3 fragPos = vec3(model*vec4(vcPos,1.0f));

  vec2 uv = vec2((atan(normal.y, normal.x) / M_PI + 1.0) * 0.5,
                                  (asin(normal.z) / M_PI + 0.5));
  //uv = vec2(atan( normal.y, normal.x )/2*M_PI, asin( normal.z )/M_PI);

  vec3 detail = texture(dTexture, uv).rgb;

  float hL = height(uv - texOffset.xz);
  float hR = height(uv + texOffset.xz);
  float hD = height(uv - texOffset.zy);
  float hU = height(uv + texOffset.zy);

  vec3 N = normalize(vec3(hL - hR, hD - hU, 2.0));
  vec3 norm = normalize(normal);
  vec3 up = vec3(0, 1, 0)-norm;
  vec3 tang = normalize(cross(norm, up));//might need flipping
  vec3 biTan = normalize(cross(norm, tang));//same
  mat3 localAxis = mat3(tang, biTan, norm);
  norm = normalize(localAxis * normalize(N));

  mat3 TBN = transpose(mat3(tang, biTan, norm));
  vec3 viewDir = normalize((viewPos*TBN)-(fragPos*TBN));
  uv = ParallaxMapping(uv,  viewDir);
//  if(uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
//        discard;
  vec3 parallax = texture(nTexture,uv).rgb;
  vec3 light = dirLighting(detail, norm);
  light*=0.8f;
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

  fColor = vec4(light+ambient*detail, 1.0f);

  float diffP = max(dot(light, parallax), 0.0);
  vec3 diffuse = diffP*ambient;

  vec3 reflectDir = reflect(-light, parallax);
  vec3 halfwayDir = normalize(light + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  vec3 specular = vec3(0.2) * spec;

  vec4 difSpe = vec4(diffuse+spec,1.0f);

  fColor = mix(fColor,difSpe,0.3333f);


  //fColor = vcColor;
}


/*
	vec3 blending = getTriPlanarBlend(vNormal);
	vec3 xaxis = texture2D( tNormal, vPos.yz * normalRepeat).rgb;
	vec3 yaxis = texture2D( tNormal, vPos.xz * normalRepeat).rgb;
	vec3 zaxis = texture2D( tNormal, vPos.xy * normalRepeat).rgb;
	vec3 normalTex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
	normalTex = normalTex * 2.0 - 1.0;
	normalTex.xy *= normalScale;
	normalTex = normalize( normalTex );

	vec3 T = vec3(0.,1.,0.);
  	vec3 BT = normalize( cross( vNormal, T ) * 1.0 );

  	mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( vNormal ) );
  	vec3 N = tsb * normalTex;*/
