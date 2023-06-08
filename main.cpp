#include"Graphics.h"
#include"Shader.h"

// Variables and Objects declaration-----------------------------------------------------------------------------------
int windowSize_x = 1000;
int windowSize_y = 1000;
float wc_x;
float wc_y;
float Scale_x = 0.1f;
float Scale_y = Scale_x;

vector<float> vertices = {
	0.5f, -0.5f, 0.0f,    0.5f, 0.0f, 0.0f,     1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 0.5f, 0.0f,     0.0f, 0.0f,
	0.0f, 0.5f, 0.0f,     0.0f, 0.0f, 0.5f,     0.5f, 1.0f
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
	1, 2, 3,
};

void cursor_pos_callBack(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_clicked(GLFWwindow* window, int button, int action, int mod);
void GetParticleState(const ParticleSystem* ps, vector<float>* dst);
void EulerSolver(ParticleSystem* ps, float DeltaT);



double previousTime{ 0.0 };
double DeltaT{ 0.0 };
double runtime{ 0.0 };

vector<float> test{};
glm::mat4 trans = glm::mat4(1.0f);

// Particle Physics ---------------------------------------------------------------------------------------------------
ParticleSystem ps;


// Main ---------------------------------------------------------------------------------------------------------------
int main()
{
	

	Particle p0(0);
	ps.AddParticle(&p0);

	// GLFW initialization --------------------------------------------------------------------------------------------
	glfwInit();
	// setting window hints aka OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setting up the window and error handling
	GLFWwindow* window = glfwCreateWindow(windowSize_x, windowSize_y, "PDBSolver", NULL, NULL);
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

	// CallBacks ------------------------------------------------------------------------------------------------------
	// updating viewport size if window size is changed CallBack
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Mouse Position CallBack
	glfwSetCursorPosCallback(window, cursor_pos_callBack);
	// Mouse Button Pressed Callback
	glfwSetMouseButtonCallback(window, mouse_clicked);
	// SpaceBar CallBack
	glfwSetKeyCallback(window, key_callback);


	// Graphical Objects Declaration ----------------------------------------------------------------------------------
	GraphicalObj rectangle(&rect, &indices);
	GraphicalObj triangle(&vertices, NULL);

	// Shader Compilation ---------------------------------------------------------------------------------------------
	Shader MainShader("./Shader.vs", "./Shader.fs");
	MainShader.use();

	// Texture Generation ---------------------------------------------------------------------------------------------
	MainShader.CreateTexture(". / Textures / GlowDot.png", "png");

	

	// Program Loop ---------------------------------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// input
		process_input(window);

		// rendering commands
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Moving the Drawn Object with Mouse
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			trans = glm::mat4(1.0f);
			trans = glm::scale(trans, glm::vec3(Scale_x, Scale_y, 1.0f));
			trans = glm::translate(trans, glm::vec3(wc_x, wc_y, 0.0f));
			unsigned int transLocation = glGetUniformLocation(MainShader.ID, "transform");
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
		}
		*/

		// Particle Physics Solver and Rendering
		runtime = glfwGetTime();
		DeltaT = runtime - previousTime;
		if (DeltaT > 0.005)
		{
			previousTime = runtime;

			EulerSolver(&ps, float(DeltaT));

			if (!ps.Particles.empty())
			{
				for (int i = 0; i < ps.n; ++i)
				{
					trans = glm::mat4(1.0f);
					trans = glm::scale(trans, glm::vec3(Scale_x, Scale_y, 1.0f));
					trans = glm::translate(trans, glm::vec3(ps.Particles[i].p[0], ps.Particles[i].p[1], 0.0f));
					unsigned int transLocation = glGetUniformLocation(MainShader.ID, "transform");
					glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
					rectangle.BufferUpdate();
					rectangle.DrawShape();
				}
			}
		}
	}

	// Unbinding and closing all glfw windows and clearing opbjects
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwTerminate();
	return 0;
}