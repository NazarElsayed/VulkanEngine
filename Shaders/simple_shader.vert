#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 colour;

layout(push_constant) uniform Push {
	vec2 offset;
	vec3 colour;
} push;

void main() {
	gl_Position	= vec4(position + push.offset, 0.0, 1.0);
}