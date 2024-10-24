# VERTEX

/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=vertex src/shaders/point_light.vert.glsl -o src/shaders/point_light.vert.glsl.spv
/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=vertex src/shaders/vertex.vert.glsl -o src/shaders/vertex.vert.glsl.spv
/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=vertex src/shaders/skybox.vert.glsl -o src/shaders/skybox.vert.glsl.spv

# FRAGMENT

/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=fragment src/shaders/point_light.frag.glsl -o src/shaders/point_light.frag.glsl.spv
/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=fragment src/shaders/fragment.frag.glsl -o src/shaders/fragment.frag.glsl.spv
/usr/bin/vulkansdk/x86_64/bin/glslc -fshader-stage=fragment src/shaders/skybox.frag.glsl -o src/shaders/skybox.frag.glsl.spv
