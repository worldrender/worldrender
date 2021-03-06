#include "include/NoiseFeedback.hpp"

std::vector<InstancedNoise> transformedData;

void instanceNoise(GLuint shader)
{
  GLuint program;
  program = attachShader(shader);
  const GLchar* feedbackOutput[] = { "outValue.vertex", "outValue.noiseValue"};
  glTransformFeedbackVaryings(program, 2, feedbackOutput, GL_INTERLEAVED_ATTRIBS);

  GLuint quadVecSize = QuadTree::verts.size();

	std::cout << "Linking program" << std::endl;
  glLinkProgram(program);

  dettachShader(program,shader);
  shaderInfo(program);
  glUseProgram(program);

  GLuint radiusID = glGetUniformLocation(program, "radius");
  GLuint scaleID = glGetUniformLocation(program, "scale");

  glUniform1f(radiusID, RADIUS);
  glUniform1f(scaleID, SCALE);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, quadVecSize * sizeof(glm::vec3), QuadTree::verts.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  GLuint feedbackBuffer;
  glGenBuffers(1, &feedbackBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer);
  glBufferData(GL_ARRAY_BUFFER, quadVecSize*sizeof(InstancedNoise), nullptr, GL_STATIC_READ);

  glEnable(GL_RASTERIZER_DISCARD);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer);
  glBeginTransformFeedback(GL_POINTS);
  glDrawArrays(GL_POINTS, 0, quadVecSize);

  glEndTransformFeedback();
  glFlush();
  transformedData.resize(quadVecSize);
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, quadVecSize*sizeof(InstancedNoise), transformedData.data());

  glDisable(GL_RASTERIZER_DISCARD);
  glDisableVertexAttribArray(0);

  glUseProgram(0);

  glDeleteBuffers(1, &feedbackBuffer);
  glDeleteBuffers(1, &vertexBuffer);

  for(InstancedNoise &elem : transformedData)
  {
    QuadTree::transformedVertices.push_back(elem.vertex);
    QuadTree::noises.push_back(elem.noiseValue);
  }
  std::cout << "Vertex Size: " << quadVecSize << std::endl;
}
