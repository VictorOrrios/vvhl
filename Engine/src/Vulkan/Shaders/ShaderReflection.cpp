
#include "ShaderReflection.hpp"

namespace vvhl {

bool ShaderReflection::initialize(std::span<const uint32_t> spirv,
                                  VkShaderStageFlagBits stage) {
  m_descriptorBindings.clear();
  m_pushConstants.clear();
  m_inputs.clear();
  m_outputs.clear();

  SpvReflectShaderModule module{};

  SpvReflectResult result =
      spvReflectCreateShaderModule(spirv.size_bytes(), spirv.data(), &module);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to create SPIR-V reflection module");
    return false;
  }

  // Descriptor bindings
  uint32_t bindingCount = 0;

  result =
      spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to enumerate descriptor bindings");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  std::vector<SpvReflectDescriptorBinding *> bindings(bindingCount);

  result = spvReflectEnumerateDescriptorBindings(&module, &bindingCount,
                                                 bindings.data());

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to reflect descriptor bindings");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  for (const auto *binding : bindings) {
    DescriptorBinding reflected{};

    reflected.set = binding->set;
    reflected.binding = binding->binding;
    reflected.descriptorType =
        static_cast<VkDescriptorType>(binding->descriptor_type);
    reflected.descriptorCount = binding->count;
    reflected.stageFlags = stage;
    reflected.defaultImageLayout = deduceImageLayout(binding);

    if (binding->name) {
      reflected.name = binding->name;
    }

    m_descriptorBindings.push_back(std::move(reflected));
  }

  // Push constants
  uint32_t pushConstantCount = 0;

  result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount,
                                                 nullptr);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to enumerate push constants");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  std::vector<SpvReflectBlockVariable *> pushConstants(pushConstantCount);

  result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount,
                                                 pushConstants.data());

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to reflect push constants");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  for (const auto *pushConstant : pushConstants) {
    PushConstantRange reflected{};

    reflected.offset = pushConstant->offset;
    reflected.size = pushConstant->size;
    reflected.stageFlags = stage;

    m_pushConstants.push_back(reflected);
  }

  // Shader inputs
  uint32_t inputCount = 0;

  result = spvReflectEnumerateInputVariables(&module, &inputCount, nullptr);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to enumerate shader inputs");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  std::vector<SpvReflectInterfaceVariable *> inputs(inputCount);

  result =
      spvReflectEnumerateInputVariables(&module, &inputCount, inputs.data());

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to reflect shader inputs");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  for (const auto *input : inputs) {
    if (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
      continue;

    ShaderInterfaceVariable reflected{};

    reflected.location = input->location;
    reflected.format = static_cast<VkFormat>(input->format);

    if (input->name)
      reflected.name = input->name;

    m_inputs.push_back(std::move(reflected));
  }

  // Shader outputs
  uint32_t outputCount = 0;

  result = spvReflectEnumerateOutputVariables(&module, &outputCount, nullptr);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to enumerate shader outputs");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  std::vector<SpvReflectInterfaceVariable *> outputs(outputCount);

  result =
      spvReflectEnumerateOutputVariables(&module, &outputCount, outputs.data());

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    LOGE("Failed to reflect shader outputs");
    spvReflectDestroyShaderModule(&module);
    return false;
  }

  for (const auto *output : outputs) {
    if (output->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
      continue;

    ShaderInterfaceVariable reflected{};

    reflected.location = output->location;
    reflected.format = static_cast<VkFormat>(output->format);

    if (output->name)
      reflected.name = output->name;

    m_outputs.push_back(std::move(reflected));
  }

  spvReflectDestroyShaderModule(&module);

  return true;
}

VkImageLayout ShaderReflection::deduceImageLayout(
    const SpvReflectDescriptorBinding *binding) const {

  // If storage => GENERAL
  if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
      binding->descriptor_type ==
          SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
    return VK_IMAGE_LAYOUT_GENERAL;
  }

  // If depth (not reliable) => DEPTH READ ONLY
  if (binding->image.depth == 1) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  }

  // Everything else => READ OPTIMAL
  return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void ShaderReflection::destroy() {
  m_descriptorBindings.clear();
  m_pushConstants.clear();
  m_inputs.clear();
  m_outputs.clear();
}

} // namespace vvhl