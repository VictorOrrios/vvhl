#pragma once

#include "ShaderReflection.hpp"

namespace vvhl {

class Shader {

public:
  Shader() = default;
  ~Shader() { destroy(); };

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  bool initialize(VkDevice device, std::string filePath,
                  VkShaderStageFlagBits stage, std::string entryPoint = "main");

  bool initialize(VkDevice device, std::span<const uint32_t> spirv,
                  VkShaderStageFlagBits stage, std::string entryPoint = "main");

  void destroy();

  bool valid() const { return m_module != VK_NULL_HANDLE; };

public:
  VkShaderModule handle() const { return m_module; };
  VkShaderStageFlagBits stage() const { return m_stage; };
  std::string_view entryPoint() const { return m_entryPoint; };
  const ShaderReflection &reflection() const { return m_reflection; };

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkShaderModule m_module = VK_NULL_HANDLE;
  VkShaderStageFlagBits m_stage{};
  std::string m_entryPoint;
  ShaderReflection m_reflection;
};

} // namespace vvhl