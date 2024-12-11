#version 450
#if defined(SUPPORT_SHADER_FLOAT16_INT8)
#extension GL_KHR_shader_float16_int8 : enable // gotta pack everything later, remember that
#extension SPV_KHR_16bit_storage : enable // For 16-bit types in storage buffers/images
#endif

/*
 * Ray Tracing (SPV_EXT_ray_tracing / VK_NV_ray_tracing):
 * If you're using ray tracing, make sure to enable these extensions for ray tracing functionality.
 * SPV_EXT_ray_tracing is for SPIR-V shaders, and VK_NV_ray_tracing is for Vulkan pipeline support.
 * 
 * Performance Monitoring (VK_KHR_shader_clock):
 * To monitor shader performance, enable VK_KHR_shader_clock. It provides time-based information
 * on shader execution which is useful for debugging and optimization.
 * 
 * Physics/Particle Simulations (VK_EXT_shader_atomic_float):
 * If you're doing physics or particle simulations and need atomic operations on floating-point values,
 * enable VK_EXT_shader_atomic_float to allow atomic operations on floats.
 *
 * Descriptor Indexing (VK_KHR_push_descriptor / SPV_EXT_descriptor_indexing):
 * These extensions provide more flexible and efficient management of resources by enabling
 * descriptor indexing and push descriptors. It's not always needed, but can be useful in complex systems.
 */

/*
 * TODO: GOD RAYs
 * 
 *
 *
 *
 */

#ifdef HAS_CEL_SHADING
#define CEL_SHADING(x,y) floor(x * y) / y
#else
#define CEL_SHADING(x,y) x
#endif

layout(constant_id = 0) const int pointLightsAmount=10;
layout(constant_id = 1) const int imagesAmount=10;
layout(constant_id = 2) const int pixelationGridSize=15;

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
  float time;
  int numLights;
} uniformBufferObject;
layout(set = 0, binding = 1) uniform sampler2D/*Array*/ image[imagesAmount];
layout(set = 0, binding = 2) uniform samplerCube enviromentMap;
layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
  float reflection;
float imageBindRepeatFactor;
  int imageBind;
  //bool isImpostor;
} push;

/*vec4 PixelMain(vec2 texCoord){
  const float decay = .97815;
  const float exposure = .92;
  const float density = .966;
  const float weight = .58767;

  const int samples = 100;

  vec4 lightPositionWorld = uniformBufferObject.view * uniformBufferObject.pointLights[0].position; // can make it iterate later
  vec4 lightPositionOnScreen = uniformBufferObject.projection * lightPositionWorld;
  lightPositionOnScreen.xyz /= lightPositionOnScreen.w; // [-1, 1];
  lightPositionOnScreen.xy = lightPositionOnScreen.xy * .5 + .5; // [0,1];

  vec2 tc = texCoord;
  vec2 deltaTexCoord = (tc - lightPositionOnScreen.xy);
  deltaTexCoord *= 1. / samples * density;
  float illuminationDecay = 1.;
  
  vec4 color = texture(image[push.imageBind], tc)*.4; 
  
  for (int i = 0; i < samples; i++){
    tc -= deltaTexCoord;
    vec4 sampler = texture(image[push.imageBind], tc)*.4; // ill have to fix this later lol
    sampler *= illuminationDecay * weight;
    color += sampler;
    illuminationDecay *= decay;
  }
  vec4 realColor = texture(ColorMapSampler, texCoord.xy);
  return vec4(vec3(color) * exposure, 1.) + realColor * 1.1;
}*/

void main(){
  vec3 diffuseLight = uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w;
  vec3 specularLight = vec3(0.);
  const vec3 surfaceNormal = normalize(fragNormalWorld);

  const vec3 cameraPositionWorld = uniformBufferObject.inverseView[3].xyz;
  const vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);

  vec3 directionalLightDirection = normalize(vec3(-7.0, -7.0, -1.0));
  vec3 directionalLightColor = vec3(1., 1., 1.);

  const float cosAngIncidence = max(dot(surfaceNormal, directionalLightDirection), 0.0);
  const float quantizedDiffuse = CEL_SHADING(cosAngIncidence, 64.);
  diffuseLight += directionalLightColor * cosAngIncidence * quantizedDiffuse;

  for (int i = 0; i < uniformBufferObject.numLights; i++){
    PointLight light = uniformBufferObject.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPositionWorld;
    float attenuation = 1./dot(directionToLight,directionToLight); // distance squared
    directionToLight = normalize(directionToLight);

    const float cosAngIncidence = max(dot(surfaceNormal,directionToLight),0);
    const float quantizedDiffuse = CEL_SHADING(cosAngIncidence, 64.);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;
  
    diffuseLight += intensity * cosAngIncidence * quantizedDiffuse;

    // specular
    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm,0,1);
    blinnTerm = pow(blinnTerm, 512.); // higher value -> higher sharpness / also make it changable by pointlight
    float quantizedSpecular = CEL_SHADING(blinnTerm, 64.);
    specularLight += intensity * blinnTerm * quantizedSpecular;
  }

  //vec2 pixelSize = vec2(1./320., 1./180.);
  //vec2 pixelatedUV = floor(fragUV / pixelSize) * pixelSize + pixelSize * .5;

  vec3 imageColor = texture(image[push.imageBind],fragUV * push.imageBindRepeatFactor/*pixelatedUV*/).rgb;

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

  hdrColor = CEL_SHADING(hdrColor, 64.);
  
  // bloom
  float brightnessThreshold = .5;
  float brightness = dot(hdrColor,vec3(0.2126, 0.7152, 0.0722));
  vec3 brightColor = (brightness > brightnessThreshold) ? hdrColor : vec3(0.0);

  outColor = vec4(hdrColor + brightColor * .5, 1.0);
}
