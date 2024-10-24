#version 450

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set=0, binding=1) uniform sampler2D image[5]; /*4->scenetexture, 5->depthtexture*/

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
layout(push_constant) uniform Push {
  vec3 fogColor;
  float fogStart;
  float fogEnd;
} push;

void main() {
    vec3 sceneColor = texture(image[3], fragUV).rgb;
    float depth = texture(image[4], fragUV).r;
    float fogFactor = clamp((push.fogEnd - depth) / (push.fogEnd - push.fogStart), 0.0, 1.0);
    vec3 finalColor = mix(push.fogColor, sceneColor, fogFactor);

    outColor = vec4(depth,depth,depth, 1.0);
}
