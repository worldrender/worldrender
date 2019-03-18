#version 430 core
#define F3 0.333333333
#define G3 0.166666667

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
//in vec2 tcTexCoord[];

out float p;
out vec3 vcNormal;
out vec4 vcColor;
//out vec2 vcTexCoord;


out vec3 tePosition;

float gx0, gy0, gz0, gx1, gy1, gz1, gx2, gy2, gz2, gx3, gy3, gz3;

float grad3lut[16][3] = {
		{ 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, // 12 cube edges
		{ -1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 1.0f },
		{ 1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, -1.0f },
		{ -1.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 1.0f }, // 4 repeats to make 16
		{ 0.0f, 1.0f, -1.0f }, { 0.0f, -1.0f, -1.0f }
};

void grad3_0(int hash) {
		int h = hash & 15;
		gx0 = grad3lut[h][0];
		gy0 = grad3lut[h][1];
		gz0 = grad3lut[h][2];
		return;
}

void grad3_1(int hash) {
		int h = hash & 15;
		gx1 = grad3lut[h][0];
		gy1 = grad3lut[h][1];
		gz1 = grad3lut[h][2];
		return;
}

void grad3_2(int hash) {
		int h = hash & 15;
		gx2 = grad3lut[h][0];
		gy2 = grad3lut[h][1];
		gz2 = grad3lut[h][2];
		return;
}

void grad3_3(int hash) {
		int h = hash & 15;
		gx3 = grad3lut[h][0];
		gy3 = grad3lut[h][1];
		gz3 = grad3lut[h][2];
		return; //aqui aidnda pra retornar
}

int perm[512] = {
	  151,160,137,91,90,15, 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
  };

vec4 dnoise(vec3 v){
	float n0, n1, n2, n3;// Noise contributions from the three corners
	// Skew the input space to determine which simplex cell we're in
	float noise;
	float s = (v.x + v.y + v.z)*F3; // Hairy factor for 2D
	float xs = v.x + s;
	float ys = v.y + s;
	float zs = v.z + s;
	int i = int(floor(xs));
	int j = int(floor(ys));
	int k = int(floor(zs));

	float t = float(i+j+k)*G3;
	float X0 = i-t; // Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	float Z0 = k-t;
	float x0 = v.x-X0; // The x,y distances from the cell origin
	float y0 = v.y-Y0;
	float z0 = v.z-Z0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1, k1;
	int i2, j2, k2; // Offsets for second (middle) corner of simplex in (i,j) coords
	if(x0>=y0) {
		if(y0>=z0)
		{ i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } /* X Y Z order */
		else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } /* X Z Y order */
		else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } /* Z X Y order */
	}
	else { // x0<y0
		if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } /* Z Y X order */
		else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } /* Y Z X order */
		else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } /* Y X Z order */
	}
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G3; /* Offsets for second corner in (x,y,z) coords */
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0f * G3; /* Offsets for third corner in (x,y,z) coords */
	float y2 = y0 - j2 + 2.0f * G3;
	float z2 = z0 - k2 + 2.0f * G3;
	float x3 = x0 - 1.0f + 3.0f * G3; /* Offsets for last corner in (x,y,z) coords */
	float y3 = y0 - 1.0f + 3.0f * G3;
	float z3 = z0 - 1.0f + 3.0f * G3;

	/* Wrap the GLuinteger indices at 256, to avoid indexing details::perm[] out of bounds */
	int ii = i & 0xff;
	int jj = j & 0xff;
	int kk = k & 0xff;

	/* Calculate the contribution from the four corners */
	float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
	float t20, t40;
	if(t0 < 0.0f) n0 = t0 = t20 = t40 = gx0 = gy0 = gz0 = 0.0f;
	else {
		grad3_0( perm[ii + perm[jj + perm[kk]]]);// <<<
		t20 = t0 * t0;
		t40 = t20 * t20;
		n0 = t40 * ( gx0 * x0 + gy0 * y0 + gz0*z0 );
	}

	float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	float t21, t41;
	if( t1 < 0.0f ) n1 = t1 = t21 = t41 = gx1 = gy1 = gz1 = 0.0f; /* No influence */
	else {
		grad3_1( perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]]);
		t21 = t1 * t1;
		t41 = t21 * t21;
		n1 = t41 * ( gx1 * x1 + gy1 * y1 + gz1 + z1);
	}

	float t2 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	float t22, t42;
	if( t2 < 0.0f ) n2 = t2 = t42 = t22 = gx2 = gy2 = gz3 = 0.0f; /* No influence */
	else {
        grad3_2( perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]]);
		t22 = t2 * t2;
		t42 = t22 * t22;
		n2 = t42 * ( gx2 * x2 + gy2 * y2 + gz2 * z2);
	}

	float t3 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	float t23, t43;
	if( t3 < 0.0f ) n3 = t3 = t43 = t23 = gx3 = gy3 = gz3 = 0.0f; /* No influence */
	else {
        grad3_3( perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]]);
		t23 = t3 * t3;
		t43 = t23 * t23;
		n3 = t43 * ( gx3 * x3 + gy3 * y3 + gz3 * z3);
	}

	noise = 28.0f * (n0 + n1 + n2 + n3);

	float temp0 = t20 * t0 * ( gx0 * x0 + gy0 * y0 + gz0 * z0 );
	float dnoise_dx = temp0 * x0;
	float dnoise_dy = temp0 * y0;
	float dnoise_dz = temp0 * z0;
	float temp1 = t21 * t1 * ( gx1 * x1 + gy1 * y1 + gz1 * z1 );
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;
	dnoise_dz += temp1 * z1;
	float temp2 = t22 * t2 * ( gx2 * x2 + gy2 * y2 + gz2 * z2 );
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;
	dnoise_dz += temp2 * z2;
	float temp3 = t23 * t3 * ( gx3 * x3 + gy3 * y3 + gz3 * z3 );
	dnoise_dx += temp3 * x3;
	dnoise_dy += temp3 * y3;
	dnoise_dz += temp3 * z3;
	dnoise_dx *= -8.0f;
	dnoise_dy *= -8.0f;
	dnoise_dz *= -8.0f;
	dnoise_dx += t40 * gx0 + t41 * gx1 + t42 * gx2 + t43 * gx3;
	dnoise_dy += t40 * gy0 + t41 * gy1 + t42 * gy2 + t43 * gy3;
	dnoise_dz += t40 * gz0 + t41 * gz1 + t42 * gz2 + t43 * gz3;
	dnoise_dx *= 28.0f; /* Scale derivative to match the noise scaling */
	dnoise_dy *= 28.0f;
	dnoise_dz *= 28.0f;

	return vec4( noise, dnoise_dx, dnoise_dy, dnoise_dz );
}

float iqfBm(vec3 v, int octaves, float lacunarity, float gain )
{
	float sum	= 0.0;
	float amp	= 1.f;
	float dx	= 0.0;
	float dy	= 0.0;
	float dz    = 0.0; //
	float freq	= 1.0;
	for( int i = 0; i < octaves; i++ ) {
		vec4 d = dnoise( v * freq );//
		dx += d.y;
		dy += d.y;
        dz += d.z;
		sum += amp * d.x / ( 1.0 + dx*dx + dy*dy + d.z*d.z);
		freq *= lacunarity;
		amp *= gain;
	}

	return sum;
}

void main(){
    vec3 tcPos0 = (tcPosition[0]);// tcPos0.y = iqfBm(tcPos0, 3, 8, 8)*radius;
    vec3 tcPos1 = (tcPosition[1]);// tcPos1.y = iqfBm(tcPos1, 3, 8, 8)*radius;
    vec3 tcPos2 = (tcPosition[2]);// tcPos2.y = iqfBm(tcPos2, 3, 8, 8)*radius;

    vec3 p0 = gl_TessCoord.x * tcPos0;
    vec3 p1 = gl_TessCoord.y * tcPos1;
    vec3 p2 = gl_TessCoord.z * tcPos2;
    tePosition = (p0 + p1 + p2);
    //tePosition*= radius;
    //tePosition.y = iqfBm(tePosition, 1,2,0.5);

    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];
    vcNormal = normalize(n0 + n1 + n2);

    vec4 c0 = gl_TessCoord.x * tcColor[0];
    vec4 c1 = gl_TessCoord.y * tcColor[1];
    vec4 c2 = gl_TessCoord.z * tcColor[2];
    vcColor = (c0 + c1 + c2);

    /*vec2 t0 = gl_TessCoord.x * tcTexCoord[0];
    vec2 t1 = gl_TessCoord.y * tcTexCoord[1];
    vec2 t2 = gl_TessCoord.z * tcTexCoord[2];
    vcTexCoord = (t0 + t1 + t2);
*/
    p = tePosition.y;
    gl_Position = MVP * vec4(tePosition, 1.0);
}

