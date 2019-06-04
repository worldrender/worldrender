#include "include/Frustum.hpp"

#include <GL/gl.h>
#include <iostream>

using namespace glm;
using namespace std;

int counter = 0;

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
cout << "OI" << counter++ << endl;
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

void Frustum::ContainsQuad(const QuadTree &quad)
{
  cout << "OI" << counter++ << endl;
	float a = quad.getQuad().c0;
	float b = quad.getQuad().c1;
	float c = quad.getQuad().c2;
	float d = quad.getQuad().c3;
	cout << "OI" << counter++ << endl;
	const glm::vec3 v0 = QuadTree::transformedVertices.at(a);
	const glm::vec3 v1 = QuadTree::transformedVertices.at(b);
	const glm::vec3 v2 = QuadTree::transformedVertices.at(c);
	const glm::vec3 v3 = QuadTree::transformedVertices.at(d);
	char rejects = 0;
	for (auto plane : Planes)
	{
		rejects = 0;

		if(QuadTree::visibility[a]==nullptr)
		  QuadTree::visibility[a] = new Visibility(dot(plane.n, v0 - plane.d));
    if(QuadTree::visibility[a] < 0)
      rejects++;

		if(QuadTree::visibility[b]==nullptr)
		  QuadTree::visibility[b] = new Visibility(dot(plane.n, v1 - plane.d));
    if(QuadTree::visibility[b] < 0)
      rejects++;

		if(QuadTree::visibility[c]==nullptr)
		  QuadTree::visibility[c] = new Visibility(dot(plane.n, v2 - plane.d));
    if(QuadTree::visibility[c] < 0)
      rejects++;

		if(QuadTree::visibility[d]==nullptr)
		  QuadTree::visibility[d] = new Visibility(dot(plane.n, v3 - plane.d));
    if(QuadTree::visibility[d] < 0)
      rejects++;
	}
    // if all three are outside a plane the triangle is outside the frustrum
  if (rejects >= 4)
  {
    return;
  }
  else if(quad.isLeaf())
  {
    this->ContainsQuad(quad.getNw());
    this->ContainsQuad(quad.getSw());
    this->ContainsQuad(quad.getNe());
    this->ContainsQuad(quad.getSe());
  }

  cout << "OI" << counter++ << endl;

  GLuint i0 = QuadTree::visibleVerts.addVertex(v0).index;
    if(i0>=QuadTree::vNoises.size())
    {
      cout << "OI" << counter++ << endl;
      QuadTree::vNoises.push_back(QuadTree::noises.at(a));
    }

  GLuint i1 = QuadTree::visibleVerts.addVertex(v1).index;
    if(i1>=QuadTree::vNoises.size())
    {
      cout << "OI" << counter++ << endl;
      QuadTree::vNoises.push_back(QuadTree::noises.at(b));
    }
  GLuint i2 = QuadTree::visibleVerts.addVertex(v2).index;
    if(i2>=QuadTree::vNoises.size())
    {
      cout << "OI" << counter++ << endl;
      QuadTree::vNoises.push_back(QuadTree::noises.at(c));
    }
  GLuint i3 = QuadTree::visibleVerts.addVertex(v3).index;
    if(i3>=QuadTree::vNoises.size())
    {
      cout << "OI" << counter++ << endl;
      QuadTree::vNoises.push_back(QuadTree::noises.at(d));
    }

  QuadTree::indices.push_back(i0);
  QuadTree::indices.push_back(i1);
  QuadTree::indices.push_back(i2);

  QuadTree::indices.push_back(i2);
  QuadTree::indices.push_back(i3);
  QuadTree::indices.push_back(i0);


}
