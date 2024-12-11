VULKAN_SDK="/usr/bin/vulkansdk/x86_64/bin"

SHADERS_DIR="src/shaders"

find "$SHADERS_DIR" -type f -name "*.glsl" | while read shader_path; do
  if [[ "$shader_path" == *.vert.glsl ]]; then
    shader_stage="vertex"
  elif [[ "$shader_path" == *.frag.glsl ]]; then
    shader_stage="fragment"
  else
    continue
  fi

  output_path="${shader_path%.glsl}.glsl.spv"

  echo "Compiling $shader_path to $output_path"
  $VULKAN_SDK/glslc -fshader-stage=$shader_stage $shader_path -o $output_path
done

echo "Shader compilation done!"

