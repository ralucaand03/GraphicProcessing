#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

out vec4 fPosEye;        // Position in eye space
out vec3 fNormal;        // Transformed normal vector
out vec2 fTexCoords;     // Texture coordinates
out vec4 fragPosLightSpace; // Position in light space

uniform mat4 model;          // Model matrix
uniform mat4 view;           // View matrix
uniform mat4 projection;     // Projection matrix
uniform mat4 lightSpaceTrMatrix; // Light space transformation matrix

void main() 
{
    // Transform vertex position into clip space
    gl_Position = projection * view * model * vec4(vPosition, 1.0f);

    // Transform vertex position into eye space
    fPosEye = view * model * vec4(vPosition, 1.0f);

    // Transform normal into eye space (normalize for safety)
    fNormal = normalize(mat3(view * model) * vNormal);

    // Transform vertex position into light space for shadow mapping
    fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);

    // Pass through texture coordinates
    fTexCoords = vTexCoords;
}
