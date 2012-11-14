#version 150
in vec3 in_position;
in vec3 in_normal;
out vec4 ex_normal;

uniform mat4 MVP;
uniform mat4 translation;

void main(){

    vec4 v = vec4(in_position, 1);
	gl_Position = MVP * v;
	vec3 n = normalize(in_normal);
	ex_normal = vec4(n, 1);

}