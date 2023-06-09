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
	GraphicalObj(Shader &shader);
	~GraphicalObj();
	void VertexUpdate(vector<float>* vertices, vector<int>* indices);
	void BufferUpdate();
	void DrawShape(glm::vec3 color);
	void transform(glm::vec3 scale, glm::vec3 translate, glm::float32 rotate, glm::vec3 rotAxis);
	
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
	Shader shader;
	GLuint VBO{}, VAO{}, EBO{};
};
#endif // !GUI_H