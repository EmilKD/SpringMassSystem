const char* fragmentShaderSource = #version 330 core\n
out vec4 fragColor;
in vec3 outColor;
void main()
{\n
	fragColor = vec4(outColor, 1.0f);
}
