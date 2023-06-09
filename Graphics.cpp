#include"Graphics.h"


// Graphical Object Class Functions -----------------------------------------------------------------------------------
GraphicalObj::GraphicalObj(Shader &shader)
{	
	this->shader = shader;
	BufferUpdate();
}

GraphicalObj::~GraphicalObj()
{
	glDeleteVertexArrays(1, &this->VBO);
}

void GraphicalObj::VertexUpdate(vector<float>*vertices, vector<int>*indices = NULL)
{
	vertexBuffer.clear();
	indexBuffer.clear();

	for (float v : *vertices)
		vertexBuffer.push_back(v);

	if (indices != nullptr)
	{
		for (int i : *indices)
			indexBuffer.push_back(i);
	}
}

void GraphicalObj::BufferUpdate()
{
	glGenBuffers(1, &this->VBO);
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->EBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), static_cast<const void*>(vertexBuffer.data()), GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);

	if (!indexBuffer.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), static_cast<const void*>(indexBuffer.data()), GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
}

void GraphicalObj::DrawShape(glm::vec3 color)
{
	this->shader.use();
	
	// Setting the Color
	this->shader.set3fv("myColor", color);

	if (!indexBuffer.empty())
	{
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else
	{
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}


void GraphicalObj::transform(glm::vec3 scale, glm::vec3 translate, glm::float32 rotate, glm::vec3 rotAxis)
{
	model = glm::mat4(1.0f);
	model = glm::scale(model, scale);
	model = glm::translate(model, translate);
	unsigned int transLocation = glGetUniformLocation(this->shader.ID, "transform");
	glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model));
}