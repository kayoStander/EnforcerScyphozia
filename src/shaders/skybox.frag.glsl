#version 450

layout(constant_id = 0) const int pointLightsAmount=10;
layout(constant_id = 1) const int imagesAmount=10;

layout(location = 0) in vec2 fragUV;

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
layout(set = 0, binding = 1) uniform sampler2D images[imagesAmount];
//layout(set = 0, binding = 3) uniform samplerCube environmentMap;
layout(push_constant) uniform Push {
  vec3 fogColor;
  float fogStart;
  float fogEnd;
} push;
float LengthSquared(vec3 vector3){
  return dot(vector3,vector3);
}

vec3 CalculateVolumetricLight(vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 lightColor){
  float lightDistance = length(lightPos - fragPos);
  float attenuation = 1. / (lightDistance * lightDistance);
  float fogFactor = smoothstep(push.fogStart, push.fogEnd, lightDistance);
  
  float samples = 10.;
  float totalLight = 0.;
  vec3 currentPos = fragPos;
  float stepSize = lightDistance / samples;
  for (int i = 0; i < int(samples); i++){
    currentPos += viewDir * stepSize;
    float dist = length(currentPos - fragPos);
    float scatter = exp(-dist / lightDistance);
    totalLight += attenuation * scatter;
  }
  
  totalLight = min(totalLight, 1.);
  
  return lightColor * totalLight * fogFactor;
}

vec3 ReconstructWorldPosition(vec2 fragUV){
  float depth = gl_FragCoord.z;
  vec4 clipSpacePos = vec4(fragUV * 2. - 1., depth * 2. - 1., 1.);
  vec4 viewSpacePos = uniformBufferObject.inverseView * clipSpacePos;
  
  vec3 fragPos = viewSpacePos.xyz / viewSpacePos.w;
  return fragPos;
}

vec3 CalculateSun(vec3 fragPos, vec3 viewDir){
  float sunAngle = uniformBufferObject.time / 2.;
  vec3 sunPos = vec3(sin(sunAngle) * 2., cos(sunAngle) * 1.5, 2.);

  vec3 sunDir = normalize(sunPos - fragPos);
  float sunIntensity = max(dot(viewDir, sunDir), 0.);

  vec3 sunColor = mix(vec3(1.,0.,.5), vec3(.1,.2,.5), .5 + .5 * cos(sunAngle));
  sunColor *= sunIntensity;
  return sunColor;
}

const float exposure = 0.3f;
const float decay = 0.96815 ; // 0.96815 
const float density  = 0.926;
const float weight  = 0.1; // 0.587
const int NUM_SAMPLES = 100;

void main() {
  vec4 textureColor = texture(images[4],fragUV);
  vec3 fragPos = ReconstructWorldPosition(fragUV);
  vec3 viewDir = normalize(fragPos);
  vec3 finalColor = textureColor.rgb;
  
  for (int i = 0;i < pointLightsAmount; i++){
    PointLight pointLight = uniformBufferObject.pointLights[i];
    vec3 lightDir = normalize(pointLight.position.xyz - fragPos);
    
    finalColor += CalculateVolumetricLight(fragPos, lightDir, pointLight.position.xyz, pointLight.color.rgb);

    vec4 lightClipSpace = uniformBufferObject.projection * uniformBufferObject.view * pointLight.position;
    vec2 lightPositionOnScreen = (lightClipSpace.xy / lightClipSpace.w) * 0.5 + 0.5; // Map to [0,1] range
  
    vec2 deltaTexCoord = vec2(fragUV.xy - lightPositionOnScreen.xy);
    vec2 texCoord = fragUV;
    deltaTexCoord *= 1. / NUM_SAMPLES * density;
    float illuminationDecay = 1.;

    for (int j = 0; j < NUM_SAMPLES; j++){
      texCoord -= deltaTexCoord;
      vec3 sampled = texture(images[4], texCoord).rgb;
      sampled *= illuminationDecay * weight;
      finalColor += sampled * .25;
      illuminationDecay *= decay;
    }
  }

  float fogFactor = smoothstep(push.fogStart, push.fogEnd, length(fragPos));
  finalColor = mix(finalColor, push.fogColor, fogFactor);
  outColor = vec4(finalColor * exposure /* * (sin(uniformBufferObject.time / 50. + .15))*/, 1.);//vec4(z,z-.1,z,1.);//mix(vec4(.1,.1,.155,0.),textureColor*,fogFactor);
}
