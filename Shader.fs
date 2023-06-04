#version 330 core
out vec4 fragColor;

in vec3 outColor;
in vec2 TexCoord;

uniform sampler2D TextureParam;

void main()
{
	//fragColor = vec4(outColor, 1.0f);
	fragColor = texture(TextureParam, TexCoord);
}
