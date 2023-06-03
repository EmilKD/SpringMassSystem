#include"GUI.h"
#include"Shader.h"

// Prototype Functinos (Callbacks)
static float g_xpos, g_ypos;
static bool left_mouse_button;

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
}

static void mouse_clicked(GLFWwindow* window, int button, int action, int mod)
{
	if (button == GLFW_PRESS)
		left_mouse_button = true;
	else if (button == GLFW_RELEASE)
		left_mouse_button = false;
}

// Graphical Object Class Functions
GraphicalObj::GraphicalObj(vector<float>* vertices, vector<int>* indices)
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


void GraphicalObj::VertexUpdate(vector<float>*vertices, vector<int>*indices)
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
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

// Coordinate Transformation Functions 


// Circle function needed


// Main -----------------------------------------------------------------------------------------------------------------------------------
int main()
{
	// Variables
	GLuint vertexShader{};
	GLuint fragShader{};
	int windowSize_x = 1000;
	int windowSize_y = 1000;
	float WindowCoords_x;
	float windowCoords_y;

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

	// updating viewport size if window size is changed CallBack
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Mouse Position CallBack
	glfwSetCursorPosCallback(window, cursor_pos_callBack);
	// Mouse Button Pressed Callback
	//glfwSetMouseButtonCallback(window, mouse_clicked);
	// Shader Compilation
	Shader MainShader("E:/projects/PBD/Shader.vs", "E:/projects/PBD/Shader.fs");
	MainShader.use();

	// Variables and Objects declaration ----------------------------------------------------------------------------------
	vector<float> vertices =
	{
		0.8f, -0.5f, 0.0f,
		1.0f, -0.5f, 0.0f,
		0.9f, 0.5f, 0.0f
	};

	vector<float> rect = {
		0.05f, 0.05f, 0.0f, 0.5f, 0.0f, 0.0f, //top right
		0.05f, -0.05f, 0.0f, 0.0f, 0.5f, 0.0f, //bottom right
		-0.05f, -0.05f, 0.0f, 0.0f, 0.0f, 0.5f, //top left
		-0.05f, 0.05f, 0.0f, 0.25f, 0.25f, 0.0f  //bottom left
	};

	vector<int> indices = {
		0, 1, 3,
		1, 2, 3
	};

	GraphicalObj rectangle(&rect, &indices);
	GraphicalObj triangle(&vertices, NULL);

	static float RecScale_x = 1;
	static float RecScale_y = RecScale_x;

	// Particle Physics ---------------------------------------------------------------------------------------------------
	ParticleSystem ps;

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

		WindowCoords_x = 2 * (g_xpos / windowSize_x) - 1;
		windowCoords_y = -2 * (g_ypos / windowSize_y) + 1;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			rect = {
					WindowCoords_x + 0.1f * RecScale_x, windowCoords_y + 0.1f * RecScale_y, 0.0f, 0.5f, 0.0f, 0.0f, //top right
					WindowCoords_x + 0.1f * RecScale_x, windowCoords_y - 0.1f * RecScale_y, 0.0f, 0.0f, 0.5f, 0.0f, //bottom right
					WindowCoords_x - 0.1f * RecScale_x, windowCoords_y - 0.1f * RecScale_y, 0.0f, 0.0f, 0.0f, 0.5f, //top left
					WindowCoords_x - 0.1f * RecScale_x, windowCoords_y + 0.1f * RecScale_y, 0.0f, 0.25f, 0.25f, 0.0f //bottom left
			};
			rectangle.VertexUpdate(&rect, &indices);

			
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
