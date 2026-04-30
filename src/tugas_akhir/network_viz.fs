#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float objectAlpha;
uniform float emissiveStrength;
uniform bool useTexture;
uniform bool isText;
uniform sampler2D texture1;

void main()
{
    if (isText) {
        vec4 texColor = texture(texture1, TexCoords);
        if (texColor.r < 0.1) discard; // Assuming white text on black background
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    vec3 baseColor;
    if (useTexture) {
        baseColor = texture(texture1, TexCoords).rgb;
    } else {
        baseColor = objectColor;
    }

    // Ambient
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor * baseColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Emissive (for glowing packets)
    vec3 emissive = emissiveStrength * baseColor;

    vec3 result = ambient + diffuse + specular + emissive;
    FragColor = vec4(result, objectAlpha);
}
