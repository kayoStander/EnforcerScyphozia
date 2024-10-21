#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUV;

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
layout(set = 0, binding = 1) uniform sampler2D image;
layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix; 
} push;

void main(){
  vec3 diffuseLight = uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w;
  vec3 specularLight = vec3(0.);
  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPositionWorld = uniformBufferObject.inverseView[3].xyz;
  vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);

  for (int i = 0; i < uniformBufferObject.numLights; i++){
    PointLight light = uniformBufferObject.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPositionWorld;
    float attenuation = 1./dot(directionToLight,directionToLight); // distance squared
    directionToLight = normalize(directionToLight);

    float cosAngIncidence = max(dot(surfaceNormal,directionToLight),0); 
    vec3 intensity = light.color.xyz * light.color.w * attenuation;
  
    diffuseLight += intensity * cosAngIncidence;

    // specular
    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm,0,1);
    blinnTerm = pow(blinnTerm, 512.); // higher value -> higher sharpness / also make it changable by pointlight
    specularLight += intensity * blinnTerm;
  }

  vec3 imageColor = texture(image,fragUV).rgb;

  outColor = vec4((diffuseLight * fragColor + specularLight * fragColor) * imageColor, 1.0);  
}
