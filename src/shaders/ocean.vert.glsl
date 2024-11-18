#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(constant_id = 0) const int pointLightsAmount=10;

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
layout(push_constant) uniform Push{
  float speed;
  float height;
  float wlength;
  float time;
  bool insideWater;
} push;
vec3 quad[4] = vec3[](
  vec3(-1.,.0,-1.),
  vec3(-1.,.0,1.),
  vec3(1.,.0,1.),
  vec3(1.,.0,-1.)
);
int indices[6] = int[](
  0,1,2,2,3,0
);

float GetWaveHeight(vec2 pos){
  return sin(pos.x / push.wlength + push.time * push.speed) * push.height;
}

void main(){
  //if (push.insideWater) {
  //}
  //float displacement = GetWaveHeight(position.xz);
  //position.y += displacement;

  gl_Position = uniformBufferObject.projection * uniformBufferObject.view * vec4(position,1.);
}
