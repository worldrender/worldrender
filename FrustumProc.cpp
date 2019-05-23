#include "Frustum.h"
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

enum FrustumStatus
{
    OUTSIDE       0,
    INSIDE        1,
    INTERSECTING  2
};

// Creates a frustum based on current OpenGL matrices
Frustum::Frustum()
{
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
	normalizePlane(frustum_planes[RIGHT]);

	frustum_planes[LEFT][A] = clip[3] + clip[0];
	frustum_planes[LEFT][B] = clip[7] + clip[4];
	frustum_planes[LEFT][C] = clip[11] + clip[8];
	frustum_planes[LEFT][D] = clip[15] + clip[12];
	normalizePlane(frustum_planes[LEFT]);

	frustum_planes[BOTTOM][A] = clip[3] + clip[1];
	frustum_planes[BOTTOM][B] = clip[7] + clip[5];
	frustum_planes[BOTTOM][C] = clip[11] + clip[9];
	frustum_planes[BOTTOM][D] = clip[15] + clip[13];
	normalizePlane(frustum_planes[BOTTOM]);

	frustum_planes[TOP][A] = clip[3] - clip[1];
	frustum_planes[TOP][B] = clip[7] - clip[5];
	frustum_planes[TOP][C] = clip[11] - clip[9];
	frustum_planes[TOP][D] = clip[15] - clip[13];
	normalizePlane(frustum_planes[TOP]);

	frustum_planes[BACK][A] = clip[3] - clip[2];
	frustum_planes[BACK][B] = clip[7] - clip[6];
	frustum_planes[BACK][C] = clip[11] - clip[10];
	frustum_planes[BACK][D] = clip[15] - clip[14];
	normalizePlane(frustum_planes[BACK]);

	frustum_planes[FRONT][A] = clip[3] + clip[2];
	frustum_planes[FRONT][B] = clip[7] + clip[6];
	frustum_planes[FRONT][C] = clip[11] + clip[10];
	frustum_planes[FRONT][D] = clip[15] + clip[14];
	normalizePlane(frustum_planes[FRONT]);

}

// Calculates the closest distance from a given point to a given clipping plane
double Frustum::CalculateDistanceToPlane(const int plane, const Vector3<double> &point) const
{
  // Return the point-to-plane distance
  return planes[plane].x * point.x + planes[plane].y * point.y + planes[plane].z * point.z + planes[plane].w;
}

// Normalizes the clipping planes
void Frustum::NormalizePlanes()
{
  // For each plane
  for (unsigned int i = 0; i < 6; i++)
  {
    // Normalize the plane using the length of its vector part
    const double normalLength = Vector3<double>(planes[i].x, planes[i].y, planes[i].z).GetLength();
    if (normalLength != 0.0)
      planes[i] /= normalLength;
  }
}

// Determines whether a given point is inside the frustum
bool Frustum::ContainsPoint(const Vector3<double> &point) const
{
  // For each plane; return outside if the point is behind the plane
  for (unsigned int i = 0; i < 6; i++)
    if (CalculateDistanceToPlane(i, point) <= 0.0)
      return OUTSIDE;

  // Return inside
  return INSIDE;
}

// Determines whether a given sphere is inside the frustum
int Frustum::ContainsSphere(const Vector3<double> &position, const double radius) const
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

// Determines whether a given box is inside the frustum
int Frustum::ContainsBox(const Vector3<double> &min, const Vector3<double> &max) const
{
  // Build a vector holding all box corners
  vector<Vector3<double> > points;
  for (unsigned int i = 0; i < 8; i++)
    points.push_back(Vector3<double>(i < 4 ? max.x : min.x, i % 4 < 2 ? max.y : min.y, i % 2 ? max.z : min.z));

  // Test the box as a polygon
  return ContainsPolygon(points);
}

// Determines whether a given polygon is inside the frustum
int Frustum::ContainsPolygon(const vector<Vector3<double> > &points) const
{
  // Plane counter
  int planeCount = 0;

  // Use the point-to-plane distance to calculate the number of planes the polygon is in front of
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int pointCount = 0;
    for (unsigned int j = 0; j < points.size(); j++)
      if (CalculateDistanceToPlane(i, points[j]) > 0.0)
        pointCount++;
    if (pointCount == 0)
      return OUTSIDE;
    else if (pointCount == points.size())
      planeCount++;
  }

  // Return inside if in front of all planes; otherwise intersecting
  return planeCount == 6 ? INSIDE : INTERSECTING;
}
