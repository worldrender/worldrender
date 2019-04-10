#include <vector>
#include <glm/glm.hpp>
#ifndef UTILS_HPP
#define UTILS_HPP

#define RADIUS 7
#define LODVALUE 6

template<typename T>
T fAbs(T t)
{
  return t>0 ? t : -t;
}

extern std::vector<glm::vec3> transformedVertices;

#endif
