#version 450

layout(location = 0) out vec2 fragUV;

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor;
  PointLight pointLights[10];
  int numLights;
} uniformBufferObject;

vec3 positions[8] = vec3[](
    vec3(-100.0, -100.0,  100.0), // Front-bottom-left
    vec3( 100.0, -100.0,  100.0), // Front-bottom-right
    vec3( 100.0,  100.0,  100.0), // Front-top-right
    vec3(-100.0,  100.0,  100.0), // Front-top-left
    vec3(-100.0, -100.0, -100.0), // Back-bottom-left
    vec3( 100.0, -100.0, -100.0), // Back-bottom-right
    vec3( 100.0,  100.0, -100.0), // Back-top-right
    vec3(-100.0,  100.0, -100.0)  // Back-top-left
);

vec2 uvs[6] = vec2[](
    vec2(0.0, 0.0), // Bottom-left
    vec2(1.0, 0.0), // Bottom-right
    vec2(1.0, 1.0), // Top-right
    vec2(1.0, 1.0), // Top-right
    vec2(0.0, 1.0), // Top-left
    vec2(0.0, 0.0)  // Bottom-left
);

int indices[36] = int[](
    0, 1, 2, 2, 3, 0,
    5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4,
    1, 5, 6, 6, 2, 1,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3
);

void main(){
  vec4 position = vec4(positions[indices[gl_VertexIndex]],1.);
  gl_Position = uniformBufferObject.projection * mat4(mat3(uniformBufferObject.view)) * position;
  fragUV = uvs[gl_VertexIndex % 6];
}
