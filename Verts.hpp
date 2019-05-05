#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstring>
#include <unordered_map>
#include <vector>
#include <experimental/optional>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

class Verts {
public:
  using index_type = uint32_t;
  static_assert(std::is_unsigned<index_type>::value, "");

private:
  std::vector<glm::vec3> verts_;
  std::unordered_map<glm::vec3, index_type> vert_index_;

public:
  static constexpr index_type INVALID_INDEX =
      static_cast<index_type>(~static_cast<index_type>(0));
  enum class AddVertexResultType { ALREADY_EXISTS, ADDED };
  struct AddVertexResult final {
    index_type index = INVALID_INDEX;
    AddVertexResultType added = AddVertexResultType::ALREADY_EXISTS;
    explicit operator bool() const {
      return added == AddVertexResultType::ADDED;
    }
  };

  AddVertexResult addVertex(const glm::vec3 &vert) {
    auto it = vert_index_.find(vert);
    if (it != vert_index_.end())
      return AddVertexResult{it->second, AddVertexResultType::ALREADY_EXISTS};

    auto index = static_cast<index_type>(verts_.size());
//    if (index != verts_.size() || index == INVALID_INDEX)
//      throw std::runtime_error("index out of bounds");

    verts_.emplace_back(vert);
    try {
      vert_index_.emplace(vert, index);
    } catch (...) {
      verts_.pop_back();
      throw;
    }
    return AddVertexResult{index, AddVertexResultType::ADDED};
  }

  using opt_vec3 = std::experimental::optional<glm::vec3>;
  using opt_index_type = std::experimental::optional<index_type>;

  opt_index_type lookupPosition(const glm::vec3 &vert) const {
    auto it = vert_index_.find(vert);
    if (it != vert_index_.end())
      return it->second;
    else
      return opt_index_type{}; // std::nullopt;
  }

  opt_vec3 lookupIndex(const index_type index) const {
    if (index >= verts_.size())
      return opt_vec3{}; // std::nullopt;
    return verts_[index];
  }

  glm::vec3 lookupIndexRequired(const index_type index) const {
    if (index >= verts_.size())
      throw std::runtime_error("out of bounds");
    return verts_[index];
  }

  size_t size() const { return verts_.size(); }
  const glm::vec3 *data() const { return verts_.data(); }
};
