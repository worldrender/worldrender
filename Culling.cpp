#include "include/NoiseFeedback.hpp"
#include "include/QuadTree.hpp"
#include "include/LoadShaders.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include "include/Utils.hpp"
#include "include/Camera.hpp"
#include "include/Frustum.hpp"

GLuint frame_index = 0,
dips_texture_buffer,
all_instances_data_vao;

const int MAX_SCENE_OBJECTS = 100000;
int num_visible_instances = MAX_SCENE_OBJECTS;
void gpuCulling(GLuint shader, Camera camera, CFrustum frustum)
{
  GLuint num_visible_instances_query[2];

  glm::mat4 ProjectionMatrix = planetCamera.getProjectionMatrix(WIDTH, HEIGHT);
  glm::mat4 ViewMatrix = planetCamera.getViewMatrix();
  frustum.CalculateFrustum(ViewMatrix, ProjectionMatrix);

  GLuint program;
  program = attachShader(shader);
	const char *vars[] = { "output_instance_data1", "output_instance_data2" };
  glTransformFeedbackVaryings(program, 2, vars, GL_INTERLEAVED_ATTRIBS);

  std::cout << "Linking program" << std::endl;
  glLinkProgram(program);

  dettachShader(program,shader);
  shaderInfo(program);
  glUseProgram(program);

  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


  glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, & MVP[0][0]);
  glUniform4fv(glGetUniformLocation(program, "frustum_planes"), 6, &frustum.frustum_planes[0].x);

//queries for getting feedback from gpu - num visible instances
	glGenQueries(2, &num_visible_instances_query[0]);

	int cur_frame = frame_index % 2;
	int prev_frame = (frame_index + 1) % 2;

	//prepare feedback & query
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, dips_texture_buffer);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, num_visible_instances_query[cur_frame]);
	glBeginTransformFeedback(GL_POINTS);

	//render cloud of points which we interprent as objects data
	glBindVertexArray(all_instances_data_vao);
	glDrawArrays(GL_POINTS, 0, MAX_SCENE_OBJECTS);
	glBindVertexArray(0);

	//disable all
	glEndTransformFeedback();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glDisable(GL_RASTERIZER_DISCARD);

	//get feedback from prev frame
	num_visible_instances = 0;
	glGetQueryObjectiv(num_visible_instances_query[prev_frame], GL_QUERY_RESULT, &num_visible_instances);

	//next frame
	frame_index++;
	}

bool SphereInFrustum(vec3 &pos, float &radius, vec4 *frustum_planes){
	bool res = true;
	//test all 6 frustum planes
	for (int i = 0; i < 6; i++)
	{
		//calculate distance from sphere center to plane.
		//if distance larger then sphere radius - sphere is outside frustum
		if (frustum_planes->x * pos.x + frustum_planes->y * pos.y + frustum_planes->z * pos.z + frustum_planes->w <= -radius)
			res = false;
			//return false; //with flag works faster
	}
	return res;
}
