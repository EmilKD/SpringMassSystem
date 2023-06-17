#ifndef Graphics_H
#define Graphics_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<string_view>
#include"PDB.h"
#include"Shader.h"

using std::vector;

class GraphicalObj {

public:
	GraphicalObj(Shader &shader, const char* TexturefilePath);
	~GraphicalObj();
	void VertexUpdate(vector<float>* vertices, vector<int>* indices);
	void BufferUpdate();
	void DrawShape(glm::vec3 color);
	void transform(glm::vec3 scale, glm::vec3 translate, glm::float32 rotate = 0.0f);
	Shader getShader();

	vector<float> vertexBuffer = {
		// Postition             Colors                   Texture Coords
		0.5f, 0.5f, 0.0f,        0.0f, 0.0f, 0.0f,        1.0f, 1.0f,          //top right
		0.5f, -0.5f, 0.0f,       0.0f, 0.0f, 0.0f,        1.0f, 0.0f,          //bottom right
		-0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 0.0f,        0.0f, 0.0f,          //bottom left
		-0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,      0.0f, 1.0f           //top left
	};

	vector<int> indexBuffer = {
		0, 1, 3,
		1, 2, 3,
	};

	glm::mat4 model = glm::mat4(1.0f);

private:
	Shader Objshader;
	GLuint VBO{}, VAO{}, EBO{};
	const char* texturePath;
};
#endif // !GUI_H4