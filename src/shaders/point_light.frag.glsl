#version 450

layout(constant_id = 0) const int pointLightsAmount=10;

layout(location = 0) in vec2 fragOffset;
layout(location = 0) out vec4 outColor;

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

const float PI = 3.1415926538;
void main(){
  float distance = sqrt(dot(fragOffset,fragOffset));
  if (distance >= 1.){
    discard;
  }

  float cosDistance = .25 * (cos(distance * PI) + 1);
  vec3 hdrColor = vec3(push.color.xyz+cosDistance);

  outColor = vec4(hdrColor,cosDistance); 
}
