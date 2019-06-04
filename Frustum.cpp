#include "include/Frustum.hpp"

#include <GL/gl.h>
#include <iostream>

using namespace glm;
using namespace std;

int counter = 0;

Frustum::Frustum( const mat4 &viewMatrix, const mat4 &projectionMatrix )
{
  const mat4 &v = viewMatrix;
  const mat4 &p = projectionMatrix;

  mat4 clipMatrix;

  clipMatrix[0][0] = v[0][0 ]*p[0][0]+v[0][1]*p[1][0]+v[0][2]*p[2][0]+v[0][3]*p[3][0];
  clipMatrix[1][0] = v[0][0]*p[0][1]+v[0][1]*p[1][1]+v[0][2]*p[2][1]+v[0][3]*p[3][1];
  clipMatrix[2][0] = v[0][0]*p[0][2]+v[0][1]*p[1][2]+v[0][2]*p[2][2]+v[0][3]*p[3][2];
  clipMatrix[3][0] = v[0][0]*p[0][3]+v[0][1]*p[1][3]+v[0][2]*p[2][3]+v[0][3]*p[3][3];
  clipMatrix[0][1] = v[1][0]*p[0][0]+v[1][1]*p[1][0]+v[1][2]*p[2][0]+v[1][3]*p[3][0];
  clipMatrix[1][1] = v[1][0]*p[0][1]+v[1][1]*p[1][1]+v[1][2]*p[2][1]+v[1][3]*p[3][1];
  clipMatrix[2][1] = v[1][0]*p[0][2]+v[1][1]*p[1][2]+v[1][2]*p[2][2]+v[1][3]*p[3][2];
  clipMatrix[3][1] = v[1][0]*p[0][3]+v[1][1]*p[1][3]+v[1][2]*p[2][3]+v[1][3]*p[3][3];
  clipMatrix[0][2] = v[2][0]*p[0][0]+v[2][1]*p[1][0]+v[2][2]*p[2][0]+v[2][3]*p[3][0];
  clipMatrix[1][2] = v[2][0]*p[0][1]+v[2][1]*p[1][1]+v[2][2]*p[2][1]+v[2][3]*p[3][1];
  clipMatrix[2][2] = v[2][0]*p[0][2]+v[2][1]*p[1][2]+v[2][2]*p[2][2]+v[2][3]*p[3][2];
  clipMatrix[3][2] = v[2][0]*p[0][3]+v[2][1]*p[1][3]+v[2][2]*p[2][3]+v[2][3]*p[3][3];
  clipMatrix[0][3] = v[3][0]*p[0][0]+v[3][1]*p[1][0]+v[3][2]*p[2][0]+v[3][3]*p[3][0];
  clipMatrix[1][3] = v[3][0]*p[0][1]+v[3][1]*p[1][1]+v[3][2]*p[2][1]+v[3][3]*p[3][1];
  clipMatrix[2][3] = v[3][0]*p[0][2]+v[3][1]*p[1][2]+v[3][2]*p[2][2]+v[3][3]*p[3][2];
  clipMatrix[3][3] = v[3][0]*p[0][3]+v[3][1]*p[1][3]+v[3][2]*p[2][3]+v[3][3]*p[3][3];

  m_planes[PLANE_RIGHT].x = clipMatrix[3][0]-clipMatrix[0][0];
  m_planes[PLANE_RIGHT].y = clipMatrix[3][1]-clipMatrix[0][1];
  m_planes[PLANE_RIGHT].z = clipMatrix[3][2]-clipMatrix[0][2];
  m_planes[PLANE_RIGHT].w = clipMatrix[3][3]-clipMatrix[0][3];

  m_planes[PLANE_LEFT].x = clipMatrix[3][0]+clipMatrix[0][0];
  m_planes[PLANE_LEFT].y = clipMatrix[3][1]+clipMatrix[0][1];
  m_planes[PLANE_LEFT].z = clipMatrix[3][2]+clipMatrix[0][2];
  m_planes[PLANE_LEFT].w = clipMatrix[3][3]+clipMatrix[0][3];

  m_planes[PLANE_BOTTOM].x = clipMatrix[3][0]+clipMatrix[1][0];
  m_planes[PLANE_BOTTOM].y = clipMatrix[3][1]+clipMatrix[1][1];
  m_planes[PLANE_BOTTOM].z = clipMatrix[3][2]+clipMatrix[1][2];
  m_planes[PLANE_BOTTOM].w = clipMatrix[3][3]+clipMatrix[1][3];

  m_planes[PLANE_TOP].x = clipMatrix[3][0]-clipMatrix[1][0];
  m_planes[PLANE_TOP].y = clipMatrix[3][1]-clipMatrix[1][1];
  m_planes[PLANE_TOP].z = clipMatrix[3][2]-clipMatrix[1][2];
  m_planes[PLANE_TOP].w = clipMatrix[3][3]-clipMatrix[1][3];

  m_planes[PLANE_BACK].x = clipMatrix[3][0]-clipMatrix[2][0];
  m_planes[PLANE_BACK].y = clipMatrix[3][1]-clipMatrix[2][1];
  m_planes[PLANE_BACK].z = clipMatrix[3][2]-clipMatrix[2][2];
  m_planes[PLANE_BACK].w = clipMatrix[3][3]-clipMatrix[2][3];

  m_planes[PLANE_FRONT].x = clipMatrix[3][0]+clipMatrix[2][0];
  m_planes[PLANE_FRONT].y = clipMatrix[3][1]+clipMatrix[2][1];
  m_planes[PLANE_FRONT].z = clipMatrix[3][2]+clipMatrix[2][2];
  m_planes[PLANE_FRONT].w = clipMatrix[3][3]+clipMatrix[2][3];

  for( int i = 0; i < 6; i++ )
  {
    m_planes[i] = glm::normalize( m_planes[i] );
  }
}

void Frustum::ContainsQuad(std::shared_ptr<QuadTree> quad)
{
  cout << "OI" << counter++ << endl;
	float a = quad->getQuad()->c0;
	float b = quad->getQuad()->c1;
	float c = quad->getQuad()->c2;
	float d = quad->getQuad()->c3;
	cout << "OI" << counter++ << endl;
	const glm::vec3 v0 = QuadTree::transformedVertices.at(a);
	const glm::vec3 v1 = QuadTree::transformedVertices.at(b);
	const glm::vec3 v2 = QuadTree::transformedVertices.at(c);
	const glm::vec3 v3 = QuadTree::transformedVertices.at(d);
	char rejects = 0;
	for (auto plane : Planes)
	{
    const float pos = m_planes[i].w;
    const vec3 normal = vec3(m_planes[i]);

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
  else if(quad->isLeaf())
  {
    this->ContainsQuad(quad->getNw());
    this->ContainsQuad(quad->getSw());
    this->ContainsQuad(quad->getNe());
    this->ContainsQuad(quad->getSe());
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
