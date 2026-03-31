#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// mix value 0.6 karena digit terakhir NIM = 6 (NIM: 240533600866)
	// 40% foto profil, 60% NIM
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.6);
}
