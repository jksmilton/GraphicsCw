#version 330 core
precision highp float;

in vec2 ex_uv;
in vec4 ex_normal;

out vec4 gl_FragColor;

uniform sampler2D textureSampler;
uniform vec4 LightV;
uniform vec4 LightC;
uniform vec4 Material;


void main(void) {
	gl_FragColor = texture(textureSampler, ex_uv) + LightC * dot(LightV, ex_normal);
}