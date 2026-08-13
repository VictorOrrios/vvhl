#pragma once

#include "Pipeline.hpp"
#include "vvhl/Core/App.hpp"

namespace vvhl {

class ComputePipeline : public Pipeline {
public:
  struct CreateInfo {
    App *app;
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