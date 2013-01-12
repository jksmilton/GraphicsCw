#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_uv;
layout(location=2) in vec3 in_normal;
out vec2 ex_uv;
out vec4 ex_normal;

uniform mat4 MVP;

void main(){
    vec4 v = vec4(in_position,1);
    gl_Position = MVP * v;
	ex_uv = in_uv;
	ex_normal = vec4(in_normal,0);
}