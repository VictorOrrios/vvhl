#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

class ShaderCompiler {

  enum class ShaderLanguage { Slang, HLSL, GLSL, Unknown };

public:
  static bool compile(const std::string &sourcePath,
                      VkShaderStageFlagBits stage,
                      const std::string &entryPoint,
                      std::vector<uint32_t> &output);

private:
  static ShaderLanguage detectLanguage(const std::string& path);

  static bool readSpirv(const std::filesystem::path &path,
                 std::vector<uint32_t> &output);

  static const char *stageToString(VkShaderStageFlagBits stage);
};

} // namespace vvhl