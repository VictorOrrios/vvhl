
#include "PipelineReflection.hpp"

namespace vvhl {
bool PipelineReflection::add(const ShaderReflection &shaderReflection) {

  // Bindings
  for (const auto &descriptor : shaderReflection.descriptorBindings()) {
    auto it =
        std::find_if(m_descriptorBindings.begin(), m_descriptorBindings.end(),
                     [&](const DescriptorBinding &existing) {
                       return existing.set == descriptor.set &&
                              existing.binding == descriptor.binding;
                     });

    if (it == m_descriptorBindings.end()) {
      m_descriptorBindings.push_back(descriptor);
      continue;
    }

    if (it->descriptorType != descriptor.descriptorType) {
      LOGE("Descriptor binding conflict: set={}, binding={}: "
           "descriptor type mismatch",
           descriptor.set, descriptor.binding);

      return false;
    }

    if (it->descriptorCount != descriptor.descriptorCount) {
      LOGE("Descriptor binding conflict: set={}, binding={}: "
           "descriptor count mismatch",
           descriptor.set, descriptor.binding);

      return false;
    }

    it->stageFlags |= descriptor.stageFlags;
  }

  // Push constants
  for (const auto &pushConstant : shaderReflection.pushConstants()) {
    if (!m_pushConstsInitialized) {
      m_pushConstants = pushConstant;
      m_pushConstsInitialized = true;
      continue;
    }

    const uint32_t begin =
        glm::min(m_pushConstants.offset, pushConstant.offset);

    const uint32_t end = glm::max(m_pushConstants.offset + m_pushConstants.size,
                                  pushConstant.offset + pushConstant.size);

    m_pushConstants.offset = begin;
    m_pushConstants.size = end - begin;
    m_pushConstants.stageFlags |= pushConstant.stageFlags;
  }

  return true;
}

void PipelineReflection::destroy() {
  m_descriptorBindings.clear();
  m_pushConstsInitialized = false;
  m_pushConstants = {};
  m_inputs.clear();
  m_outputs.clear();
}

} // namespace vvhl