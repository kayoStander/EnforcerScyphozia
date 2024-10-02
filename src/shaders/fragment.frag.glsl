#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor;
  PointLight pointLights[10];
  int numLights;
} uniformBufferObject;
layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix; 
} push;

void main(){
  vec3 diffuseLight = uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w;
  vec3 surfaceNormal = normalize(fragNormalWorld);

  for (int i = 0; i < uniformBufferObject.numLights; i++){
    PointLight light = uniformBufferObject.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPositionWorld;
    float attenuation = 1./dot(directionToLight,directionToLight); // distance squared
    float cosAngIncidence = max(dot(surfaceNormal,normalize(directionToLight)),0); 
    vec3 intensity = light.color.xyz * light.color.w * attenuation;
  
    diffuseLight += intensity * cosAngIncidence;
  }

  outColor = vec4(diffuseLight * fragColor, 1.);  
}
