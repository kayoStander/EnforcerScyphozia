#version 450

// center = 0,0

vec2 positions[3] = vec2[] (
  vec2(0.,-.5),
  vec2(.5,.5),
  vec2(-.5,.5)
); 

void main(){
  gl_Position = vec4(positions[gl_VertexIndex], 0., 1.);
}
