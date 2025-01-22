#version 410 core

out vec4 fColor;
in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, TexCoords).r;
    fColor = vec4(vec3(depthValue), 1.0f); // Visualize depth as grayscale
}
