#include "include/Frustum.hpp"
#include <GL/gl.h>

enum FrustumSide
{
	RIGHT	= 0,		// The RIGHT side of the frustum
	LEFT	= 1,		// The LEFT	 side of the frustum
	BOTTOM	= 2,		// The BOTTOM side of the frustum
	TOP		= 3,		// The TOP side of the frustum
	BACK	= 4,		// The BACK	side of the frustum
	FRONT	= 5			// The FRONT side of the frustum
};

enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};

enum FrustumStatus
{
    OUTSIDE       = 0,
    INSIDE        = 1,
    INTERSECTING  = 2
};


void Frustum::SettingFrustum(glm::mat4 proj_matrix, glm::mat4 view_matrix){
    float   *proj = &proj_matrix[0][0];
	float   *modl = &view_matrix[0][0];
	float   clip[16]; //clipping planes

	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	frustum_planes[RIGHT][A] = clip[3] - clip[0];
	frustum_planes[RIGHT][B] = clip[7] - clip[4];
	frustum_planes[RIGHT][C] = clip[11] - clip[8];
	frustum_planes[RIGHT][D] = clip[15] - clip[12];
	NormalizePlane(frustum_planes[RIGHT]);

	frustum_planes[LEFT][A] = clip[3] + clip[0];
	frustum_planes[LEFT][B] = clip[7] + clip[4];
	frustum_planes[LEFT][C] = clip[11] + clip[8];
	frustum_planes[LEFT][D] = clip[15] + clip[12];
	NormalizePlane(frustum_planes[LEFT]);

	frustum_planes[BOTTOM][A] = clip[3] + clip[1];
	frustum_planes[BOTTOM][B] = clip[7] + clip[5];
	frustum_planes[BOTTOM][C] = clip[11] + clip[9];
	frustum_planes[BOTTOM][D] = clip[15] + clip[13];
	NormalizePlane(frustum_planes[BOTTOM]);

	frustum_planes[TOP][A] = clip[3] - clip[1];
	frustum_planes[TOP][B] = clip[7] - clip[5];
	frustum_planes[TOP][C] = clip[11] - clip[9];
	frustum_planes[TOP][D] = clip[15] - clip[13];
	NormalizePlane(frustum_planes[TOP]);

	frustum_planes[BACK][A] = clip[3] - clip[2];
	frustum_planes[BACK][B] = clip[7] - clip[6];
	frustum_planes[BACK][C] = clip[11] - clip[10];
	frustum_planes[BACK][D] = clip[15] - clip[14];
	NormalizePlane(frustum_planes[BACK]);

	frustum_planes[FRONT][A] = clip[3] + clip[2];
	frustum_planes[FRONT][B] = clip[7] + clip[6];
	frustum_planes[FRONT][C] = clip[11] + clip[10];
	frustum_planes[FRONT][D] = clip[15] + clip[14];
	NormalizePlane(frustum_planes[FRONT]);
}
// Creates a frustum based on current OpenGL matrices
Frustum::Frustum(glm::mat4 proj_matrix, glm::mat4 view_matrix)
{
    SettingFrustum(proj_matrix, view_matrix);

}

// Calculates the closest distance from a given point to a given clipping plane
double Frustum::CalculateDistanceToPlane(const int plane, const glm::vec3 &point) const
{
  // Return the point-to-plane distance
  return frustum_planes[plane].x * point.x + frustum_planes[plane].y * point.y + frustum_planes[plane].z * point.z + frustum_planes[plane].w;
}

void Frustum::NormalizePlane(glm::vec4 &frustum_plane)
{
	float magnitude = (float)sqrt(frustum_plane[A] * frustum_plane[A] + frustum_plane[B] * frustum_plane[B] + frustum_plane[C] * frustum_plane[C]);
	frustum_plane[A] /= magnitude;
	frustum_plane[B] /= magnitude;
	frustum_plane[C] /= magnitude;
	frustum_plane[D] /= magnitude;
}


// Determines whether a given sphere is inside the frustum
int Frustum::ContainsSphere(const glm::vec3 &position, const double radius) const
{
  // Plane counter
  int planeCount = 0;

  // Use the point-to-plane distance to calculate the number of planes the sphere is in front of
  for (unsigned int i = 0; i < 6; i++)
  {
    const double distance = CalculateDistanceToPlane(i, position);
    if (distance <= -radius)
      return OUTSIDE;
    else if (distance > radius)
      planeCount++;
  }

  // Return inside if in front of all planes; otherwise intersecting
  return planeCount == 6 ? INSIDE : INTERSECTING;
}

bool Frustum::ContainsPoint(const glm::vec3 &point) const
{
  // For each plane; return outside if the point is behind the plane
  for (unsigned int i = 0; i < 6; i++)
    if (CalculateDistanceToPlane(i, point) <= 0.0)
      return OUTSIDE;

  // Return inside
  return INSIDE;
}

void Frustum::Update(glm::mat4 proj_matrix, glm::mat4 view_matrix){
    SettingFrustum(proj_matrix, view_matrix);
}
