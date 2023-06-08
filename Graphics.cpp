#include"Graphics.h"

using std::cout, std::endl;

float g_xpos, g_ypos;
bool left_mouse_button;

GLuint vertexShader{};
GLuint fragShader{};

extern int windowSize_x;
extern int windowSize_y;
extern float wc_x;
extern float wc_y;
extern float Scale_x;
extern float Scale_y;

extern ParticleSystem ps;

// CallBacks ----------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void cursor_pos_callBack(GLFWwindow* window, double xpos, double ypos)
{
	g_xpos = xpos;
	g_ypos = ypos;
	wc_x = (2 * (xpos / windowSize_x) - 1) / Scale_x;
	wc_y = (-2 * (ypos / windowSize_y) + 1) / Scale_y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		cout << endl;
		for (Particle p : ps.Particles)
		{
			cout << "Particle #" << p.ID << " pos: " << "x:" << p.p[0] << " y:" << p.p[1] << " z:" << p.p[2] << endl << endl;
		}
		cout << "Number of Particles: " << ps.Particles.size() << endl;
	}

}

void mouse_clicked(GLFWwindow* window, int button, int action, int mod)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		array<float, 3> pos{ wc_x, wc_y, 0};
		int lastID = ps.Particles.size();
		
		Particle p(lastID, 1.0f, &pos);
		ps.AddParticle(&p);
	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		array<float, 3> pos{ wc_x, wc_y, 0};
		int lastID = ps.Particles.size();

		Particle p(lastID, 1.0f, &pos);
		ps.AddParticle(&p);

		ps.SpringParticles.push_back({ {0, lastID} });
	}
}



// Graphical Object Class Functions -----------------------------------------------------------------------------------
GraphicalObj::GraphicalObj(vector<float>* vertices, vector<int>* indices = NULL)
{	
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	for (float v : *vertices) 
		vertexBuffer.push_back(v);

	if (indices != nullptr)
	{
		for (int i : *indices)
			indexBuffer.push_back(i);
	}
}

GraphicalObj::~GraphicalObj()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), static_cast<const void*>(vertexBuffer.data()), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	if (!indexBuffer.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), static_cast<const void*>(indexBuffer.data()), GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO);
}

void GraphicalObj::DrawShape()
{
	if (indexBuffer.empty())
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}


