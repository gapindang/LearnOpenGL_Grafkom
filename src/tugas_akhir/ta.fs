#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3  lightPos;
uniform vec3  viewPos;
uniform vec3  lightColor;
uniform vec3  objectColor;   // base color tint
uniform float useTexture;    // 1.0 = use texture, 0.0 = solid color
uniform float highlightMix;  // 0.0 normal, 1.0 fully highlighted
uniform vec3  highlightColor;

uniform sampler2D texture1;

void main()
{
    // ---------- Phong Lighting ----------
    // ambient
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm     = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular   = specularStrength * spec * lightColor;

    // ---------- Base color ----------
    vec4 baseColor;
    if (useTexture > 0.5)
        baseColor = texture(texture1, TexCoord);
    else
        baseColor = vec4(objectColor, 1.0);

    // ---------- Combine ----------
    vec3 lighting = (ambient + diffuse + specular);
    vec3 result   = lighting * baseColor.rgb;

    // highlight blend
    result = mix(result, highlightColor, highlightMix);

    FragColor = vec4(result, baseColor.a);
}
