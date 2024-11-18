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
  int numLights;
} uniformBufferObject;
layout(set = 0, binding = 1) uniform sampler2D images[imagesAmount];
layout(set = 0, binding = 2) uniform samplerCube environmentMap;
layout(push_constant) uniform Push {
  vec3 fogColor;
  float fogStart;
  float fogEnd;
} push;
float LengthSquared(vec3 vector3){
  return vector3.x * vector3.x + vector3.y * vector3.y + vector3.z * vector3.z
}
void main() {
  /*
  vec2 uv = fragUV * 2.0 - 1.0;
  vec3 viewDirection = normalize(vec3(uv.x, uv.y, -1.0));
  vec3 worldDirection = normalize((mat4(mat3(uniformBufferObject.inverseView)) * vec4(viewDirection,0.)).xyz);
  */
  
  vec4 textureColor = texture(images[4],fragUV);

  /*const float density = .0005;
  const float LOG2 = 1.442695;
  float z = gl_FragCoord.z/gl_FragCoord.w;
  float fogFactor= exp2(density*density*z*z*LOG2);
  fogFactor=clamp(fogFactor,0.,1.);
*/
  outColor = textureColor;//vec4(z,z-.1,z,1.);//mix(vec4(.1,.1,.155,0.),textureColor*,fogFactor);
}
