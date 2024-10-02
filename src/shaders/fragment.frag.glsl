#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;


layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projectionViewMatrix;
  vec4 ambientLightColor;
  vec3 lightPosition;
  vec4 lightColor;
} uniformBufferObject;
layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix; 
} push;

void main(){

  vec3 directionToLight = uniformBufferObject.lightPosition - fragPositionWorld;
  float attenuation = 1./dot(directionToLight,directionToLight); // distance squared

  vec3 lightColor = uniformBufferObject.lightColor.xyz * uniformBufferObject.lightColor.w * attenuation;
  vec3 ambientLight = uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w;
  vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld),normalize(directionToLight)),0);

  outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.);  
}
