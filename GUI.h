#ifndef GUI_H
#define GUI_H


#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<string_view>
#include"PDB.h"


// Shaders
// Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 outColor;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"	outColor = aColor;\n"
"}\n";

// Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 fragColor;\n"
"in vec3 outColor;"
"void main()\n"
"{\n"
"	fragColor = vec4(outColor, 1.0f);\n"
//"	fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}";


void ShaderInit(GLuint vertexShader, GLuint fragShader)
{
	////////////////////////////////// Shaders
	// Compiling the Vertex Shader

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checking Shader Compilation Errors
	int  success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Shader Compile Error!" << infoLog << std::endl;
	}

	// Compiling the Fragment Shader

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);

	// Checking Shader Compilation Errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Shader Compile Error!" << infoLog << std::endl;
	}
	else
		std::cout << "Shader Compiled";
	// Defining the Shader Program
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	// Checking Program Linking Errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Program Linking Error!" << infoLog << std::endl;
	}
	// Using the Shader Program
	glUseProgram(shaderProgram);
	// Deleting the Shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

class GraphicalObj {
public:
	GLuint VBO, VAO, EBO;
	GraphicalObj(vector<float>* vertices, vector<int>* indices);
	~GraphicalObj();
	void VertexUpdate(vector<float>* vertices, vector<int>* indices);
	void BufferUpdate();
	void DrawShape();
	
	vector<float> vertexBuffer;
	vector<int> indexBuffer;
};
#endif // !GUI_H