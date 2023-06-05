#include"Graphics.h"
#include"Shader.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

using std::cout, std::endl;

static float g_xpos, g_ypos;
static bool left_mouse_button;
// Variables
GLuint vertexShader{};
GLuint fragShader{};
int windowSize_x = 1000;
int windowSize_y = 1000;
float wc_x;
float wc_y;
static float Scale_x = 0.2;
static float Scale_y = Scale_x;

// Particle Physics ---------------------------------------------------------------------------------------------------
ParticleSystem ps;

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

static void cursor_pos_callBack(GLFWwindow* window, double xpos, double ypos)
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
		for (Particle p : ps.Particles)
		{
			cout << "Particle #" << p.ID << " pos: " << "x:" << p.p[0] << " y:" << p.p[1] << " z:" << p.p[2] << endl << endl;
		}
		cout << "Number of Particles: " << ps.Particles.size() << endl;
	}
}

static void mouse_clicked(GLFWwindow* window, int button, int action, int mod)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		array<float, 3> pos{ wc_x, wc_y, 0};
		int lastID = ps.Particles.size();
		
		Particle p(lastID, 1.0f, &pos);
		ps.AddParticle(&p);
	}
}

// Graphical Object Class Functions
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



// Main -----------------------------------------------------------------------------------------------------------------------------------
int main()
{
	// GLFW initialization ----------------------------------------------------------------------------------
	glfwInit();
	// setting window hints aka OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setting up the window and error handling
	GLFWwindow* window = glfwCreateWindow(windowSize_x, windowSize_y, "HelloWorld", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "window failed to Initialize";
		return -1;
	}
	// setting the window as OpendGl's current context
	glfwMakeContextCurrent(window);

	// glad loading error handling
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// CallBacks -------------------------------------------------------------------------------------------------
	// updating viewport size if window size is changed CallBack
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Mouse Position CallBack
	glfwSetCursorPosCallback(window, cursor_pos_callBack);
	// Mouse Button Pressed Callback
	glfwSetMouseButtonCallback(window, mouse_clicked);
	// SpaceBar CallBack
	glfwSetKeyCallback(window, key_callback);
	

	// Shader Compilation --------------------------------------------------------------------------------------------
	Shader MainShader("./Shader.vs", "./Shader.fs");
	MainShader.use();

	// Texture Generation --------------------------------------------------------------------------------------------
	
	MainShader.CreateTexture(". / Textures / Spring.png", "png");
	
	
	// Variables and Objects declaration------------------------------------------------------------------------------
	vector<float> vertices =
	{
		0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f
	};

	vector<float> rect = {
		// Postition             Colors                   Texture Coords
		0.5f, 0.5f, 0.0f,        0.5f, 0.0f, 0.0f,        1.0f, 1.0f,          //top right
		0.5f, -0.5f, 0.0f,       0.0f, 0.5f, 0.0f,        1.0f, 0.0f,          //bottom right
		-0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 0.5f,        0.0f, 0.0f,          //bottom left
		-0.5f, 0.5f, 0.0f,       0.25f, 0.25f, 0.0f,      0.0f, 1.0f           //top left
	};

	vector<int> indices = {
		0, 1, 3,
		1, 2, 3
	};

	GraphicalObj rectangle(&rect, &indices);
	GraphicalObj triangle(&vertices);


	// While Loop ---------------------------------------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// input
		process_input(window);

		// rendering commands
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			/* Relic
			rect = {
					// Position                                      Color                       Texture Coords
					wc_x + 0.1f * Scale_x, wc_y + 0.1f * Scale_y,    0.0f, 0.5f, 0.0f, 0.0f,     1.0f, 1.0f,       //top right
					wc_x + 0.1f * Scale_x, wc_y - 0.1f * Scale_y,    0.0f, 0.0f, 0.5f, 0.0f,     1.0f, 0.0f,       //bottom right
					wc_x - 0.1f * Scale_x, wc_y - 0.1f * Scale_y,    0.0f, 0.0f, 0.0f, 0.5f,     0.0f, 0.0f,       //bottom left
					wc_x - 0.1f * Scale_x, wc_y + 0.1f * Scale_y,    0.0f, 0.25f, 0.25f, 0.0f,    0.0f, 1.0f	   //top left
			};
			rectangle.VertexUpdate(&rect, &indices);
			*/

			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::scale(trans, glm::vec3(Scale_x, Scale_y, 1.0f));
			trans = glm::translate(trans, glm::vec3(wc_x, wc_y, 0.0f));
			unsigned int transLocation = glGetUniformLocation(MainShader.ID, "transform");
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));

		}	
		
		
		// Physics Sim
		//double time = glfwGetTime();

		rectangle.BufferUpdate();
		rectangle.DrawShape();
		//triangle.BufferUpdate();
		//triangle.DrawShape();
		
	}

	// Unbinding and closing all glfw windows and clearing opbjects
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwTerminate();
	return 0;
}


