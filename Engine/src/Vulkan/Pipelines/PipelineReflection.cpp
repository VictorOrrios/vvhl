#include "PipelineReflection.hpp"

namespace vvhl {

bool PipelineReflection::add(const ShaderReflection &shaderReflection) {

  // Bindings
  for (const auto &descriptor : shaderReflection.descriptorBindings()) {
    auto &setBindings = m_descriptorBindings[descriptor.set];

    auto it = std::find_if(setBindings.begin(), setBindings.end(),
                           [&](const ReflectedDescriptorBinding &existing) {
                             return existing.binding == descriptor.binding;
                           });

    if (it == setBindings.end()) {
      setBindings.push_back(descriptor);
      continue;
    }

    if (it->descriptorType != descriptor.descriptorType) {
      LOGE("Descriptor binding conflict: set=%u, binding=%u: "
           "descriptor type mismatch (%d vs %d)",
           descriptor.set, descriptor.binding,
           uint(it->descriptorType), uint(descriptor.descriptorType));
      return false;
    }

    if (it->descriptorCount != descriptor.descriptorCount) {
      LOGE("Descriptor binding conflict: set=%u, binding=%u: "
           "descriptor count mismatch (%u vs %u)",
           descriptor.set, descriptor.binding,
           it->descriptorCount, descriptor.descriptorCount);
      return false;
    }

    it->stageFlags |= descriptor.stageFlags;
  }

  // Push constants
  for (const auto &pushConstant : shaderReflection.pushConstants()) {
    auto it = std::find_if(m_pushConstants.begin(), m_pushConstants.end(),
                           [&](const VkPushConstantRange &existing) {
                             return existing.offset == pushConstant.offset &&
                                    existing.size == pushConstant.size;
                           });

    if (it != m_pushConstants.end()) {
      it->stageFlags |= pushConstant.stageFlags;
    } else {
      m_pushConstants.push_back(pushConstant);
    }
  }

  return true;
}

void PipelineReflection::destroy() {
  m_descriptorBindings.clear();
  m_pushConstants.clear();
}

} // namespace vvhl