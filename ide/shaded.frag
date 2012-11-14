#version 150
precision highp float;

in vec4 ex_normal;
uniform vec4 LightV;
uniform vec4 LightC;
uniform vec4 Material;

out vec4 gl_FragColor;

void main(void) {
	gl_FragColor =  Material * LightC * dot(LightV, ex_normal);
}