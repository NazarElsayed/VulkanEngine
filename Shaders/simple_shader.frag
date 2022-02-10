#version 450

layout (location = 0) out vec4 outColour;

layout(push_constant) uniform Push {
	vec2 offset;
	vec3 colour;
} push;

void main () {
	//outColour = vec4(0.8f, 0.5f, 0.0f, 1.0f);	
	outColour = vec4(push.colour, 1.0f);
}