#version 150
in  vec3 in_Position;
uniform mat4 MVP;
uniform mat4 translation;
void main() {
    vec4 v = vec4( in_Position, 1.0);
	v = translation * v;
	gl_Position = MVP * v;
}