#include "include/Frustum.hpp"


using namespace glm;
using namespace std;

int counter = 0;

Frustum::Frustum( const mat4 &viewMatrix, const mat4 &projectionMatrix )
{
  const mat4 &v = viewMatrix;
  const mat4 &p = projectionMatrix;

  mat4 clipMatrix;

  clipMatrix[0][0] = v[0][0]*p[0][0]+v[0][1]*p[1][0]+v[0][2]*p[2][0]+v[0][3]*p[3][0];
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
	const float a = quad->getQuad()->c0;
	const float b = quad->getQuad()->c1;
	const float c = quad->getQuad()->c2;
	const float d = quad->getQuad()->c3;

	const glm::vec3 v0 = QuadTree::transformedVertices.at(a);
	const glm::vec3 v1 = QuadTree::transformedVertices.at(b);
	const glm::vec3 v2 = QuadTree::transformedVertices.at(c);
	const glm::vec3 v3 = QuadTree::transformedVertices.at(d);

	char rejects = 0;

	for (GLuint i=0;i<6;i++)
	{
    if(!QuadTree::visibility[a])
      QuadTree::visibility[a] = new Visibility(m_planes[i][0] * v0.x + m_planes[i][1] * v0.y + m_planes[i][2] * v0.z + m_planes[i][3]);
    if(QuadTree::visibility[a]->boundary <= 0)
      rejects++;
    if(!QuadTree::visibility[b])
      QuadTree::visibility[b] = new Visibility(m_planes[i][0] * v1.x + m_planes[i][1] * v1.y + m_planes[i][2] * v1.z + m_planes[i][3]);
    if(QuadTree::visibility[b]->boundary <= 0)
      rejects++;
    if(!QuadTree::visibility[c])
      QuadTree::visibility[c] = new Visibility(m_planes[i][0] * v2.x + m_planes[i][1] * v2.y + m_planes[i][2] * v2.z + m_planes[i][3]);
    if(QuadTree::visibility[c]->boundary <= 0)
      rejects++;
    if(!QuadTree::visibility[d])
      QuadTree::visibility[d] = new Visibility(m_planes[i][0] * v3.x + m_planes[i][1] * v3.y + m_planes[i][2] * v3.z + m_planes[i][3]);
    if(QuadTree::visibility[d]->boundary <= 0)
      rejects++;
	}
    // if all three are outside a plane the triangle is outside the frustrum
  if (rejects >= 4)
  {
    return;
  }

  if(!quad->isLeaf())
  {
    this->ContainsQuad(quad->getNw());
    this->ContainsQuad(quad->getSw());
    this->ContainsQuad(quad->getNe());
    this->ContainsQuad(quad->getSe());
  }
  else
  {
    const GLuint i0 = QuadTree::visibleVerts.addVertex(v0).index;
    if(i0>=QuadTree::vNoises.size())
      QuadTree::vNoises.push_back(QuadTree::noises.at(a));

    const GLuint i1 = QuadTree::visibleVerts.addVertex(v1).index;
    if(i1>=QuadTree::vNoises.size())
      QuadTree::vNoises.push_back(QuadTree::noises.at(b));

    const GLuint i2 = QuadTree::visibleVerts.addVertex(v2).index;
    if(i2>=QuadTree::vNoises.size())
      QuadTree::vNoises.push_back(QuadTree::noises.at(c));

    const GLuint i3 = QuadTree::visibleVerts.addVertex(v3).index;
    if(i3>=QuadTree::vNoises.size())
      QuadTree::vNoises.push_back(QuadTree::noises.at(d));

    QuadTree::indices.push_back(i0);
    QuadTree::indices.push_back(i1);
    QuadTree::indices.push_back(i2);

    QuadTree::indices.push_back(i2);
    QuadTree::indices.push_back(i3);
    QuadTree::indices.push_back(i0);
  }

}
