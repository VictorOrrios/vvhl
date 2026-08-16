
#include <vvhl/Vulkan/Shaders/ShaderCompiler.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

const char *ShaderCompiler::stageToString(VkShaderStageFlagBits stage) {
  switch (stage) {
  case VK_SHADER_STAGE_VERTEX_BIT:
    return "vertex";

  case VK_SHADER_STAGE_FRAGMENT_BIT:
    return "fragment";

  case VK_SHADER_STAGE_COMPUTE_BIT:
    return "compute";

  case VK_SHADER_STAGE_GEOMETRY_BIT:
    return "geometry";

  case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
    return "hull";

  case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
    return "domain";

  default:
    return nullptr;
  }
}

bool ShaderCompiler::readSpirv(const std::filesystem::path &path,
                               std::vector<uint32_t> &output) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file) {
    LOGE("Failed to open compiled SPIR-V: {}", path.string());
    return false;
  }

  const std::streamsize size = file.tellg();

  if (size <= 0 || size % sizeof(uint32_t) != 0) {
    LOGE("Invalid SPIR-V file size: {}", path.string());
    return false;
  }

  output.resize(static_cast<size_t>(size) / sizeof(uint32_t));

  file.seekg(0);
  file.read(reinterpret_cast<char *>(output.data()), size);

  return file.good();
}

ShaderCompiler::ShaderLanguage
ShaderCompiler::detectLanguage(const std::string &path) {
  const auto extension = std::filesystem::path(path).extension();

  if (extension == ".slang")
    return ShaderLanguage::Slang;

  if (extension == ".hlsl")
    return ShaderLanguage::HLSL;

  if (extension == ".glsl")
    return ShaderLanguage::GLSL;

  return ShaderLanguage::Unknown;
}

bool ShaderCompiler::compile(const std::string &sourcePath,
                             VkShaderStageFlagBits stage,
                             const std::string &entryPoint,
                             std::vector<uint32_t> &output) {

  ShaderLanguage lang = detectLanguage(sourcePath);

  output.clear();

  const char *stageName = stageToString(stage);

  if (!stageName) {
    LOGE("Unsupported shader stage");
    return false;
  }

  if (!std::filesystem::exists(sourcePath)) {
    LOGE("Shader source does not exist: {}", sourcePath);
    return false;
  }

  const auto tempPath =
      std::filesystem::temp_directory_path() /
      ("vvhl_shader_" + std::to_string(std::hash<std::string>{}(sourcePath)) +
       ".spv");

  std::string compiler;
  std::string command;

  const std::string optimization =
      BuildConfig::EnableShaderCompilerOptimization ? " -O3" : "";

  switch (lang) {
  case ShaderLanguage::Slang:
  case ShaderLanguage::HLSL:
    compiler = "slangc";

    command = compiler + " \"" + sourcePath + "\"" + " -target spirv" +
              " -stage " + stageName + " -entry " + entryPoint + optimization +
              " -o \"" + tempPath.string() + "\"";

    break;

  case ShaderLanguage::GLSL:
    compiler = "glslangValidator";

    command = compiler + " -V \"" + sourcePath + "\"" + " -S " +
              std::string(stageName == std::string("vertex")     ? "vert"
                          : stageName == std::string("fragment") ? "frag"
                          : stageName == std::string("compute")  ? "comp"
                                                                 : "") +
              optimization + " -o \"" + tempPath.string() + "\"";

    break;

  case ShaderLanguage::Unknown:
    LOGE("Unknown shader language, file extension not recognized");
    return false;
  }

  LOGI("Compiling shader: {}", sourcePath);

  const int result = std::system(command.c_str());

  if (result != 0) {
    LOGE("Shader compilation failed: {}", sourcePath);
    std::filesystem::remove(tempPath);
    return false;
  }

  const bool success = readSpirv(tempPath, output);

  std::filesystem::remove(tempPath);

  if (!success) {
    LOGE("Failed to read compiled SPIR-V: {}", sourcePath);
    return false;
  }

  return true;
}

} // namespace vvhl