
#include <vvhl/Vulkan/Shaders/Shader.hpp>
#include <vvhl/Vulkan/Shaders/ShaderCompiler.hpp>

namespace vvhl {

bool Shader::initialize(VkDevice device, std::string filePath,
                        VkShaderStageFlagBits stage, std::string entryPoint) {

  std::vector<uint32_t> spirv;
  if (!ShaderCompiler::compile(filePath, stage, entryPoint, spirv))
    return false;

  return initialize(device, spirv, stage, entryPoint);
}

bool Shader::initialize(VkDevice device, std::span<const uint32_t> spirv, VkShaderStageFlagBits stage,
                        std::string entryPoint) {

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = spirv.size() * sizeof(uint32_t);
  createInfo.pCode = reinterpret_cast<const uint32_t *>(spirv.data());

  if (vkCreateShaderModule(device, &createInfo, nullptr, &m_module) !=
      VK_SUCCESS) {
    LOGE("Failed to create shader module")
    m_module = VK_NULL_HANDLE;
    return false;
  }

  if (!m_reflection.initialize(spirv,stage)) {
    return false;
  }

  m_entryPoint = entryPoint;
  m_stage = stage;
  m_device = device;

  return true;
}

void Shader::destroy() {
  if (!valid())
    return;
  vkDestroyShaderModule(m_device, m_module, nullptr);

  m_reflection.destroy();
  m_device = VK_NULL_HANDLE;
  m_module = VK_NULL_HANDLE;
  m_stage = {};
  m_entryPoint = "";
}

} // namespace vvhl