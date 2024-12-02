#version 450

layout(constant_id = 0) const int pointLightsAmount=10;

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.,-1.),
  vec2(-1.,1.),
  vec2(1.,-1.),
  vec2(1.,-1.),
  vec2(-1.,1.),
  vec2(1.,1.)
);

layout(location = 0) out vec2 fragOffset;

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
  float time;
  int numLights;
} uniformBufferObject;
layout(push_constant) uniform Push{
  vec4 position;
  vec4 color; 
  float radius;
} push;

void main(){
  fragOffset = OFFSETS[gl_VertexIndex];
  vec3 cameraRightWorld = {uniformBufferObject.view[0][0], uniformBufferObject.view[1][0], uniformBufferObject.view[2][0]};
  vec3 cameraUpWorld = {uniformBufferObject.view[0][1], uniformBufferObject.view[1][1], uniformBufferObject.view[2][1]};

  vec3 positionWorld = push.position.xyz 
    + push.radius * fragOffset.x * cameraRightWorld 
    + push.radius * fragOffset.y * cameraUpWorld;

  gl_Position = uniformBufferObject.projection * uniformBufferObject.view * vec4(positionWorld,1.);
}
