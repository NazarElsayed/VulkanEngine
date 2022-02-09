#version 450

layout (location = 0) in vec3 fragColour;

layout (location = 0) out vec4 outColour;

void main () {
	//outColour = vec4(0.8f, 0.5f, 0.0f, 1.0f);	
	outColour = vec4(fragColour, 1.0f);
}