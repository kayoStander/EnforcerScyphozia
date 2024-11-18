#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const int pointLightsAmount=10;
layout(constant_id = 1) const int imagesAmount=10;

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
  PointLight pointLights[pointLightsAmount];
  //float deltaTime;
  int numLights;
} uniformBufferObject;
layout(set = 0, binding = 1) uniform sampler2D/*Array*/ image[imagesAmount];
layout(set = 0, binding = 2) uniform samplerCube enviromentMap;
layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
  float reflection;
  int imageBind;
  //bool isImpostor;
} push;

void main(){
  vec3 diffuseLight = uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w;
  vec3 specularLight = vec3(0.);
  const vec3 surfaceNormal = normalize(fragNormalWorld);

  const vec3 cameraPositionWorld = uniformBufferObject.inverseView[3].xyz;
  const vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);

  vec3 directionalLightDirection = normalize(vec3(-7.0, -7.0, -1.0));
  vec3 directionalLightColor = vec3(1., 1., 1.);

  float cosAngIncidence = max(dot(surfaceNormal, directionalLightDirection), 0.0);
  diffuseLight += directionalLightColor * cosAngIncidence;

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

  vec3 imageColor = texture(image[push.imageBind],fragUV).rgb;

  // reflection
  vec3 reflectionDirection = reflect(viewDirection,surfaceNormal);
  vec2 reflectionCoordinates = normalize(reflectionDirection).xy * .5 + .5;
  vec3 reflectionColor = texture(image[4],reflectionCoordinates).rgb;

  // fog
  float fogStart = 10.; // make dynamic later
  float fogEnd = 50.; // make dynamic later
  float distance = length(cameraPositionWorld - fragPositionWorld);
  float fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart),0.,1.);

  vec3 hdrColor = mix(vec3(.1,.1,.155),(diffuseLight * fragColor + specularLight * fragColor) * imageColor +
                      push.reflection * reflectionColor,fogFactor);
  
  // bloom
  float brightnessThreshold = .5;
  float brightness = dot(hdrColor,vec3(0.2126, 0.7152, 0.0722));
  vec3 brightColor = (brightness > brightnessThreshold) ? hdrColor : vec3(0.0);

  outColor = vec4(hdrColor + brightColor * .25, 1.0);
}
