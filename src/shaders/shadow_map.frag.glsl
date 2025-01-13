#version 450

layout(location = 0) in vec4 fragPosLightSpace; 

layout(set = 0, binding = 2) uniform sampler2D shadowMap;  

layout(location = 0) out vec4 outColor;

void main() {
  float depth = fragPosLightSpace.z / fragPosLightSpace.w;
  outColor = vec4(vec3(depth), 1.0); // Visualize depth
}
