
#include "Pipeline.hpp"
#include "Vulkan/Descriptors/DescriptorBinding.hpp"
#include "Vulkan/Descriptors/DescriptorSet.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Shaders/Shader.hpp"
#include "vvhl/Core/EngineConfig.hpp"
#include <cstdint>

namespace vvhl {

bool Pipeline::writeAllFrames(DescriptorBinding resourceBind) {
  for (uint32_t i = 0; i < EngineSettings::maxFramesInFlight(); i++) {
    if (!write(resourceBind, i))
      return false;
  }
  return true;
}

bool Pipeline::write(DescriptorBinding resourceBind, const uint32_t frameSet) {
  ReflectedDescriptorBinding rdb = {};
  uint32_t set_id, bind_id;
  bool failed = false;

  std::visit(
      [&](const auto &id) {
        using T = std::decay_t<decltype(id)>;

        if constexpr (std::is_same_v<T, std::string>) {
          if (!m_reflection.findByName(id, rdb)) {
            LOGE("Failed at finding binding named {}", id)
            failed = true;
          } else {
            set_id = rdb.set;
            bind_id = rdb.binding;
          }
        } else if constexpr (std::is_same_v<T, std::pair<uint32_t, uint32_t>>) {
          if (!m_reflection.findById(id.first, id.second, rdb)) {
            LOGE("Failed at finding binding {} at set {}", id.first, id.second)
            failed = true;
          } else {
            set_id = id.first;
            bind_id = id.second;
          }
        }
      },
      resourceBind.binding);

  if (failed)
    return false;

  std::visit(
      [&](const auto &resource) {
        using T = std::decay_t<decltype(resource)>;

        if constexpr (std::is_same_v<T, BufferBinding>) {
          BufferWriteDescriptor bw = {.handle = resource.handle,
                                      .descriptorType = rdb.descriptorType,
                                      .offset = resource.offset,
                                      .range = resource.range};
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, bw);

        } else if constexpr (std::is_same_v<T, ImageBinding>) {
          ImageWriteDescriptor iw = {.handle = resource.handle,
                                     .descriptorType = rdb.descriptorType};
          if (resource.layout.has_value())
            iw.imageLayout = resource.layout.value();
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, iw);
        } else if constexpr (std::is_same_v<T, SamplerBinding>) {
          SamplerWriteDescriptor sw = {
              .handle = resource.handle,
          };
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, sw);
        } else if constexpr (std::is_same_v<T, CombinedImageSamplerBinding>) {
          CombinedImageSamplerWriteDescriptor cw = {
              .imageHandle = resource.imageHandle,
              .samplerHandle = resource.samplerHandle,
          };
          if (resource.layout.has_value())
            cw.imageLayout = resource.layout.value();
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, cw);
        } else if constexpr (std::is_same_v<T, std::vector<BufferBinding>>) {
          std::vector<BufferWriteDescriptor> bws;
          bws.reserve(resource.size());
          for (auto &bb : resource) {
            bws.push_back({.handle = bb.handle,
                           .descriptorType = rdb.descriptorType,
                           .offset = bb.offset,
                           .range = bb.range});
          }
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, bws);
        } else if constexpr (std::is_same_v<T, std::vector<ImageBinding>>) {
          std::vector<ImageWriteDescriptor> iws;
          iws.reserve(resource.size());
          for (auto &ib : resource) {
            ImageWriteDescriptor iw = {.handle = ib.handle,
                                       .descriptorType = rdb.descriptorType};
            if (ib.layout.has_value())
              iw.imageLayout = ib.layout.value();
            iws.push_back(iw);
          }
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, iws);
        } else if constexpr (std::is_same_v<T, std::vector<SamplerBinding>>) {
          std::vector<SamplerWriteDescriptor> sws;
          sws.reserve(resource.size());
          for (auto &sb : resource) {
            sws.push_back({.handle = sb.handle});
          }
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, sws);
        } else if constexpr (std::is_same_v<
                                 T, std::vector<CombinedImageSamplerBinding>>) {
          std::vector<CombinedImageSamplerWriteDescriptor> cws;
          cws.reserve(resource.size());
          for (auto &cb : resource) {
            CombinedImageSamplerWriteDescriptor cw = {
                .imageHandle = cb.imageHandle,
                .samplerHandle = cb.samplerHandle,
            };
            if (cb.layout.has_value())
              cw.imageLayout = cb.layout.value();
            cws.push_back(cw);
          }
          m_descriptorGroups[set_id].sets[frameSet].write(bind_id, cws);
        }
      },
      resourceBind.resource);

  return true;
}

void Pipeline::updateDescriptors() {
  for (auto &[setNumber, group] : m_descriptorGroups) {
    for (auto &set : group.sets) {
      set.update();
    }
  }
}

bool Pipeline::resolveBinding(const BindingId &binding, uint32_t &set,
                              uint32_t &bindingIndex) {
  ReflectedDescriptorBinding rdb;

  bool found = false;

  std::visit(
      [&](const auto &id) {
        using T = std::decay_t<decltype(id)>;

        if constexpr (std::is_same_v<T, std::string>) {
          found = m_reflection.findByName(id, rdb);
          if (found) {
            set = rdb.set;
            bindingIndex = rdb.binding;
          } else {
            LOGE("Binding '{}' not found in pipeline reflection", id);
          }
        } else if constexpr (std::is_same_v<T, std::pair<uint32_t, uint32_t>>) {
          found = m_reflection.findById(id.first, id.second, rdb);
          if (found) {
            set = id.first;
            bindingIndex = id.second;
          } else {
            LOGE("Binding ({}, {}) not found in pipeline reflection", id.first,
                 id.second);
          }
        }
      },
      binding);

  return found;
}

bool Pipeline::attachmentSetup() {

  auto bindingsBySet = m_reflection.descriptorBindings();
  std::vector<VkDescriptorSetLayout> vkLayouts;

  for (const auto &[set, bindings] : bindingsBySet) {
    DescriptorSetLayout layout;
    if (!layout.initialize(m_device, bindings)) {
      return false;
    }
    vkLayouts.push_back(layout.handle());

    m_descriptorGroups[set] = {.layout = std::move(layout),
                               .sets = std::vector<DescriptorSet>()};
  }

  if (!createLayout(vkLayouts, m_reflection.pushConstants())) {
    return false;
  }

  return true;
}

void Pipeline::accumulatePoolResources(DescriptorPool &pool) const {
  for (const auto &[set, bindings] : m_reflection.descriptorBindings()) {
    for (const auto &binding : bindings) {
      DescriptorPool::PoolSize poolSize{};
      poolSize.type = binding.descriptorType;
      poolSize.descriptorCount = binding.descriptorCount;
      pool.accumulate(poolSize);
    }
    pool.accumulateSet(1);
  }
}

bool Pipeline::createLayout(
    std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
    std::span<const VkPushConstantRange> pushConstantRanges) {

  VkPipelineLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  createInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  createInfo.pSetLayouts = descriptorSetLayouts.data();
  createInfo.pushConstantRangeCount =
      static_cast<uint32_t>(pushConstantRanges.size());
  createInfo.pPushConstantRanges =
      pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();

  if (vkCreatePipelineLayout(m_device, &createInfo, nullptr, &m_layout) !=
      VK_SUCCESS) {
    LOGE("Failed to create pipeline layout")
    return false;
  }

  return true;
}

bool Pipeline::createShader(const ShaderInput &input,
                            const std::string &entryPoint,
                            const VkShaderStageFlagBits stage, Shader &shader) {
  shader.destroy();

  bool success = false;

  std::visit(
      [&](const auto &source) {
        using T = std::decay_t<decltype(source)>;

        if constexpr (std::is_same_v<T, ShaderSource>) {
          success = shader.initialize(m_device, source, stage, entryPoint);
        } else if constexpr (std::is_same_v<T, ShaderBinary>) {
          success = shader.initialize(m_device, source, stage, entryPoint);
        }
      },
      input);

  if (!success || !shader.valid()) {
    return false;
  }

  if (!m_reflection.add(shader.reflection())) {
    return false;
  }

  return true;
}

} // namespace vvhl