#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

class Shader {
public:
  Shader() = default;
  ~Shader() { destroy(); };

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  bool create();

  void destroy();

public:


private:
  VkShaderModule m_module;
  
};

} // namespace vvhl