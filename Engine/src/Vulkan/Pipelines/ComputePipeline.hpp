#pragma once

#include "Pipeline.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {

class ComputePipeline : public Pipeline {
public:
  struct CreateInfo {
    VkDevice device;
    ShaderInput shaderInput;
    std::string entryPoint = "main";
    DescriptorPool *descriptorPool = nullptr;
    VkPipelineCreateFlags flags = 0;
  };

public:
  ComputePipeline() = default;
  ~ComputePipeline() = default;

  ComputePipeline(const ComputePipeline &) = delete;
  ComputePipeline &operator=(const ComputePipeline &) = delete;

  bool initialize(const CreateInfo &createInfo);

protected:
  bool createPipeline() override;

private:
  Shader m_shader;
  VkPipelineCreateFlags m_flags = 0;
};

} // namespace vvhl