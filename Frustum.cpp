#include "include/Frustum.hpp"

#include <GL/gl.h>

using namespace glm;

void FrustumCorners::Transform(mat4 space)
{
	//move corners of the near plane
	this->na = (space*vec4(na, 0));
	this->nb = (space*vec4(nb, 0));
	this->nc = (space*vec4(nc, 0));
	this->nd = (space*vec4(nd, 0));
	//move corners of the far plane
	this->fa = (space*vec4(fa, 0));
	this->fb = (space*vec4(fb, 0));
	this->fc = (space*vec4(fc, 0));
	this->fd = (space*vec4(fd, 0));
}

Frustum::Frustum()
{
	this->Corners = FrustumCorners();
}
Frustum::~Frustum()
{
}

//create transforms to prevent transforming every triangle into world space
void Frustum::SetCullTransform(mat4 objectWorld)
{
	CullWorld = objectWorld;
	CullInverse = glm::inverse(objectWorld);
}

void Frustum::SetToCamera(Camera* camera)
{
	Position = camera->Position;
	Forward = camera->Front;
	Up = camera->Up;
	Right = camera->Right;
	NearPlane = camera->Near;
	FarPlane = camera->Far;
	FOV = camera->Zoom;
}

void Frustum::Update()
{
	float normHalfWidth = glm::tan(glm::radians(FOV));
	float aspectRatio = ASPECT_RATIO;

	//calculate width and height for near and far plane
	float nearHW = normHalfWidth*NearPlane;
	float nearHH = nearHW / aspectRatio;
	float farHW = normHalfWidth*FarPlane;// *0.5f;
	float farHH = farHW / aspectRatio;

	//calculate near and far plane centers
	auto nCenter = Position + Forward*NearPlane;
	auto fCenter = Position + Forward*FarPlane *0.5f;

	//construct corners of the near plane in the culled objects world space
	Corners.na = nCenter + Up*nearHH - Right*nearHW;
	Corners.nb = nCenter + Up*nearHH + Right*nearHW;
	Corners.nc = nCenter - Up*nearHH - Right*nearHW;
	Corners.nd = nCenter - Up*nearHH + Right*nearHW;

	Corners.fa = fCenter + Up*farHH - Right*farHW;
	Corners.fb = fCenter + Up*farHH + Right*farHW;
	Corners.fc = fCenter - Up*farHH - Right*farHW;
	Corners.fd = fCenter - Up*farHH + Right*farHW;

	Corners.Transform(CullInverse);

	PositionObject = (CullInverse*vec4(Position, 0)).xyz();
	RadInvFOV = 1 / radians(FOV);

	//construct planes
	Planes.clear();
	//winding in an outside perspective so the cross product creates normals pointing inward
	Planes.push_back(Plane(Corners.na, Corners.nb, Corners.nc));//Near
	Planes.push_back(Plane(Corners.fb, Corners.fa, Corners.fd));//Far
	Planes.push_back(Plane(Corners.fa, Corners.na, Corners.fc));//Left
	Planes.push_back(Plane(Corners.nb, Corners.fb, Corners.nd));//Right
	Planes.push_back(Plane(Corners.fa, Corners.fb, Corners.na));//Top
	Planes.push_back(Plane(Corners.nc, Corners.nd, Corners.fc));//Bottom*/
}

void Frustum::ContainsQuad(QuadTree *quad)
{
	float a = quad->getQuad().c0;
	float b = quad->getQuad().c1;
	float c = quad->getQuad().c2;
	float d = quad->getQuad().c3;
	char rejects = 0;
	for (auto plane : Planes)
	{
		rejects = 0;

		if(QuadTree::visibility[a]==nullptr)
		  QuadTree::visibility[a] = new Visibility(dot(plane.n, QuadTree::verts.lookupIndexRequired(a) - plane.d));
    if(QuadTree::visibility[a] < 0)
      rejects++;

		if(QuadTree::visibility[b]==nullptr)
		  QuadTree::visibility[b] = new Visibility(dot(plane.n, QuadTree::verts.lookupIndexRequired(b) - plane.d));
    if(QuadTree::visibility[b] < 0)
      rejects++;

		if(QuadTree::visibility[c]==nullptr)
		  QuadTree::visibility[c] = new Visibility(dot(plane.n, QuadTree::verts.lookupIndexRequired(c) - plane.d));
    if(QuadTree::visibility[c] < 0)
      rejects++;

		if(QuadTree::visibility[d]==nullptr)
		  QuadTree::visibility[d] = new Visibility(dot(plane.n, QuadTree::verts.lookupIndexRequired(d) - plane.d));
    if(QuadTree::visibility[d] < 0)
      rejects++;
	}
    // if all three are outside a plane the triangle is outside the frustrum
  if (rejects >= 4)
  {
    quad->setCulling();
    return;
  }
  else if(quad->isLeaf())
  {
    this->ContainsQuad(quad->getNw());
    this->ContainsQuad(quad->getSw());
    this->ContainsQuad(quad->getNe());
    this->ContainsQuad(quad->getSe());
  }
  QuadTree::quadTreeList.push_back(quad);
}
