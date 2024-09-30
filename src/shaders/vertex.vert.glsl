#version 450

// center = 0,0

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 transform;
  vec3 color; 
} push;

void main(){
  //gl_Position = vec4(push.transform * position, 0., 1.);
  gl_Position = push.transform * vec4(position,1.);
  fragColor = color;
}
