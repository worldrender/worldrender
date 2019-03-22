
#include "NoiseFeedback.hpp"
#include "QuadTree.hpp"
#include "LoadShaders.hpp"
#include <iostream>
#include <vector>
#include <numeric>

std::vector<InstancedNoise> instancedNoise;

void instanceNoise(GLuint shader)
{
  GLuint program;
  program = attachShader(shader);
  const GLchar* feedbackOutput[] = { "outValue" };
  glTransformFeedbackVaryings(program, 1, feedbackOutput, GL_INTERLEAVED_ATTRIBS);

  GLuint quadVecSize = QuadTree::vertices.size();
  std::vector<GLuint> quadIndex(quadVecSize);

  std::iota (std::begin(quadIndex), std::end(quadIndex), 0);


	printf("Linking program\n");
  glLinkProgram(program);

  dettachShader(program,shader);

  glUseProgram(program);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint feedbackBuffer;
  glGenBuffers(1, &feedbackBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer);
  glBufferData(GL_ARRAY_BUFFER, quadVecSize * sizeof(glm::vec3), QuadTree::vertices.data(), GL_STATIC_DRAW);

  GLint vertexAttrib = glGetAttribLocation(program, "vertex");
  glEnableVertexAttribArray(vertexAttrib);
  glVertexAttribPointer(vertexAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

  GLuint indexBuffer;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ARRAY_BUFFER, quadVecSize * sizeof(GLuint), quadIndex.data(), GL_STATIC_DRAW);

  GLint indexAttrib = glGetAttribLocation(program, "index");
  glEnableVertexAttribArray(indexAttrib);
  glVertexAttribPointer(indexAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

  GLuint tbo;
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, quadVecSize*sizeof(InstancedNoise), nullptr, GL_STATIC_READ);

  glEnable(GL_RASTERIZER_DISCARD);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
  glBeginTransformFeedback(GL_POINTS);
  glDrawArrays(GL_POINTS, 0, quadVecSize);

  glEndTransformFeedback();
  glFlush();
  instancedNoise.resize(quadVecSize);
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, quadVecSize*sizeof(InstancedNoise), instancedNoise.data());

  glDisable(GL_RASTERIZER_DISCARD);

  glUseProgram(0);

  glDeleteBuffers(1, &indexBuffer);
  glDeleteBuffers(1, &tbo);
  glDeleteBuffers(1, &feedbackBuffer);

  glDeleteVertexArrays(1,&vao);
}
