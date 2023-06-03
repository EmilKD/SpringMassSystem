#ifndef GUI_H
#define GUI_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<string_view>
#include"PDB.h"

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