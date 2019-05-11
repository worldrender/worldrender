#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include <GL/gl.h>
//#include "..\glext\glext.h"
#include <glm/glm.hpp>

class Camera;
class CFrustum
{
public:
	CFrustum(){};
	~CFrustum(){};

	void CalculateFrustum(glm::mat4 &view_matrix, glm::mat4 &proj_matrix);

	glm::vec4 frustum_planes[6];
};

void gpuCulling(GLuint shader, Camera camera, CFrustum frustum);

#endif
