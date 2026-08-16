#pragma once

#include "Pipeline.hpp"
#include <vvhl/RenderPass/RenderPass.hpp>

namespace vvhl {

inline uint32_t calcGroupCounts(uint32_t size, uint32_t workgroupSize) {
  return (size + (workgroupSize - 1)) / workgroupSize;
}

inline VkExtent2D calcGroupCounts(const VkExtent2D &size,
                                  const VkExtent2D &workgroupSize) {
  return VkExtent2D{
      (size.width + (workgroupSize.width - 1)) / workgroupSize.width,
      (size.height + (workgroupSize.height - 1)) / workgroupSize.height};
}

inline VkExtent3D calcGroupCounts(const VkExtent3D &size,
                                  const VkExtent3D &workgroupSize) {
  return VkExtent3D{
      (size.width + (workgroupSize.width - 1)) / workgroupSize.width,
      (size.height + (workgroupSize.height - 1)) / workgroupSize.height,
      (size.depth + (workgroupSize.depth - 1)) / workgroupSize.depth};
}

inline VkExtent2D calcGroupCounts(const VkExtent2D &size,
                                  uint32_t workgroupSize) {
  return calcGroupCounts(size, VkExtent2D(workgroupSize, workgroupSize));
}

inline VkExtent3D calcGroupCounts(const VkExtent3D &size,
                                  uint32_t workgroupSize) {
  return calcGroupCounts(
      size, VkExtent3D(workgroupSize, workgroupSize, workgroupSize));
}

class ComputePipeline : public Pipeline {
public:
  struct CreateInfo {
    RenderPass *renderPass;
    ShaderInput shaderInput;
    VkPipelineCreateFlags flags = 0;
  };

public:
  ComputePipeline() = default;
  ~ComputePipeline() = default;

  ComputePipeline(const ComputePipeline &) = delete;
  ComputePipeline &operator=(const ComputePipeline &) = delete;

  bool initialize(const CreateInfo &createInfo);

  virtual void destroy() override;

  void dispatch(VkCommandBuffer cmd, uint32_t groupCountX, uint32_t groupCountY,
                uint32_t groupCountZ) const;

protected:
  bool createPipeline() override;

private:
  Shader m_shader;
  VkPipelineCreateFlags m_flags = 0;
};

} // namespace vvhl