#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = inColor * texture(texSampler, inTexCoord);
	//outColor = vec4(1, 1, 1, 1);
}
