#version 450

// center = 0,0

layout(constant_id = 0) const int pointLightsAmount=10;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUV; 

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor;
  PointLight pointLights[pointLightsAmount];
  int numLights;
} uniformBufferObject;
layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix; 
  float reflection;
  int imageBind; 
} push;

void main(){
  vec4 positionWorld = push.modelMatrix * vec4(position,1.);
  gl_Position = uniformBufferObject.projection * uniformBufferObject.view * positionWorld;
  fragNormalWorld = normalize(mat3(push.normalMatrix)*normal);
  fragPositionWorld = positionWorld.xyz;
  fragColor = color;
  fragUV = uv;
}
