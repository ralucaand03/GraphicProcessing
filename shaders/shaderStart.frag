#version 410 core

in vec4 fPosEye;             // Position in eye space
in vec3 fNormal;             // Transformed normal vector
in vec2 fTexCoords;          // Texture coordinates
in vec4 fragPosLightSpace;   // Position in light space for shadow mapping

out vec4 fColor;

// Uniforms
uniform mat3 normalMatrix;       // Normal matrix
uniform vec3 lightDir;           // Light direction
uniform vec3 lightColor;         // Light color
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform float fogDensity;

// Lighting parameters
vec3 ambient;
vec3 diffuse;
vec3 specular;
float ambientStrength = 0.2f;
float specularStrength = 0.5f;

void computeDirLight() {
    vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 lightDirN = normalize(lightDir);
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Ambient light
    ambient = ambientStrength * lightColor;

    // Diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    // Specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0);
    specular = specularStrength * specCoeff * lightColor;
}

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

    if (normalizedCoords.z > 1.0f) return 0.0f; // Outside light frustum

    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;

    // Bias to prevent shadow acne (dynamic bias based on surface angle)
    float bias = max(0.005f * (1.0 - dot(normalize(fNormal), normalize(lightDir))), 0.001f);

    // Hard shadow (can be enhanced with PCF for smoothness)
    return (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;
}

float computeFog() {
    float fogFactor = exp(-pow(length(fPosEye.xyz) * fogDensity, 2.0));
    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() {
    computeDirLight();

    vec3 texDiffuse = texture(diffuseTexture, fTexCoords).rgb;
    vec3 texSpecular = texture(specularTexture, fTexCoords).rgb;

    vec3 colorAmbient = ambient * texDiffuse;
    vec3 colorDiffuse = diffuse * texDiffuse;
    vec3 colorSpecular = specular * texSpecular;

    float shadow = computeShadow();
    vec3 lighting = colorAmbient + (1.0 - shadow) * (colorDiffuse + colorSpecular);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    fColor = mix(fogColor, vec4(lighting, 1.0f), fogFactor);
}
