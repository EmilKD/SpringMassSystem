#include"Graphics.h"
#include"Shader.h"

using std::cout, std::endl, std::vector, std::array;

// Particle Physics ---------------------------------------------------------------------------------------------------
ParticleSystem ps;

// Variables and Objects declaration-----------------------------------------------------------------------------------
float g_xpos, g_ypos;
bool left_mouse_button;

array<int, 2> windowSize{1000, 1000};
float wc_x;
float wc_y;

float Scale_x = 0.05f;
float Scale_y = Scale_x;

double previousTime{ 0.0 };
double DeltaT{ 0.0 };
double runtime{ 0.0 };
int timer{ 0 };

enum ObjectType
{
	Mass,
	Spring
};

struct Colors
{
	glm::vec3 Amber{glm::vec3(1.0f, 0.75f, 0.0f)};
	glm::vec3 White{glm::vec3(1.0f, 1.0f, 1.0f)};
};

void GetParticleState(const ParticleSystem* ps, vector<float>* dst);
void EulerSolver(ParticleSystem* ps, float DeltaT);

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
	wc_x = (2 * (xpos / windowSize[0]) - 1) / Scale_x;
	wc_y = (-2 * (ypos / windowSize[1]) + 1) / Scale_y;
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
		glm::vec3 pos{wc_x, wc_y, 0};
		glm::float32 velMag = 0;
		glm::vec3 vel = velMag * glm::cross(glm::normalize(pos), glm::vec3(0.0f, 0.0f, 1.0));
		

		int lastID = ps.n;

		Particle p(lastID, 0.1f, &pos, &vel);
		ps.AddParticle(&p);
	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		glm::vec3 pos{ wc_x, wc_y, 0};
		glm::float32 velMag = 0;
		glm::vec3 vel = velMag * glm::cross(glm::normalize(pos), glm::vec3(0.0f, 0.0f, 1.0));
		

		int lastID = ps.n;

		Particle p(lastID, 0.1f, &pos, &vel);
		ps.AddParticle(&p);

		ps.SpringParticles.push_back(lastID - 1);
		ps.SpringParticles.push_back(lastID);
	}
}


// Main ---------------------------------------------------------------------------------------------------------------
int main()
{
	glm::vec3 pos0, pos1, pos2, pos3;
	pos0 = glm::vec3(-2, 4, 0);
	pos1 = glm::vec3(2, 4, 0);
	pos2 = glm::vec3(-2, 2, 0);
	pos3 = glm::vec3(2, 2, 0);

	Particle p0(0, 1.0f, &pos0);
	ps.AddParticle(&p0);
	Particle p1(1, 1.0f, &pos1);
	ps.AddParticle(&p1);
	Particle p2(2, 1.0f, &pos2);
	ps.AddParticle(&p2);
	Particle p3(3, 1.0f, &pos3);
	ps.AddParticle(&p3);

	ps.Ignoreparticles.push_back(0);
	ps.Ignoreparticles.push_back(1);

	ps.SpringParticles.push_back(0);
	ps.SpringParticles.push_back(1);
	ps.SpringParticles.push_back(0);
	ps.SpringParticles.push_back(2);
	ps.SpringParticles.push_back(0);
	ps.SpringParticles.push_back(3);
	ps.SpringParticles.push_back(1);
	ps.SpringParticles.push_back(2);
	ps.SpringParticles.push_back(1);
	ps.SpringParticles.push_back(3);
	ps.SpringParticles.push_back(2);
	ps.SpringParticles.push_back(3);

	// GLFW initialization --------------------------------------------------------------------------------------------
	glfwInit();
	// setting window hints aka OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// setting up the window and error handling
	GLFWwindow* window = glfwCreateWindow(windowSize[0], windowSize[1], "PDBSolver", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "window failed to Initialize";
		return -1;
	}

	// setting the window as OpendGl's current context
	glfwMakeContextCurrent(window);

	//Turning VSync Off! :/
	glfwSwapInterval(0);

	// glad loading error handling
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// CallBacks ------------------------------------------------------------------------------------------------------
	// updating viewport size if window size is changed CallBack
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callBack);
	glfwSetMouseButtonCallback(window, mouse_clicked);
	glfwSetKeyCallback(window, key_callback);

	// Shader Compilation ---------------------------------------------------------------------------------------------
	Shader MainShader;
	

	// Graphical Objects Declaration ----------------------------------------------------------------------------------
	GraphicalObj rectangle(MainShader);
	Colors color;

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

		// Particle Physics Solver and Rendering
		runtime = glfwGetTime();
		DeltaT = runtime - previousTime;

		//if (DeltaT > 0.005)
		{
			previousTime = runtime;

			ps.EulerSolve(float(DeltaT));

			if (!ps.Particles.empty())
			{
				for (int i = 0; i < ps.n; ++i)
				{
					/*if (i == 0)
					{
						rectangle.transform(glm::vec3(Scale_x*2.0f, Scale_y*2.0f, 1.0f), glm::vec3(ps.Particles[0].p[0], ps.Particles[0].p[1], 0.0f), 30, glm::vec3(1.0f));
						rectangle.DrawShape(color.Amber);

					}*/

					//else
					{
						rectangle.transform(glm::vec3(Scale_x, Scale_y, 1.0f), glm::vec3(ps.Particles[i].p[0], ps.Particles[i].p[1], 0.0f), 30, glm::vec3(1.0f));
						rectangle.DrawShape(color.White);
					}
				}
			}
		}

		//if (timer > 500)
		//{
		//	//system("CLS");
		//	cout << "FPS: " << 1 / DeltaT << endl;
		//	timer = 0;
		//}

		if (timer > 5000)
		{
			//ps.DeleteParticle(ps.n);
			timer = 0;
		}
		++timer;
	}

	// Unbinding and closing all glfw windows and clearing opbjects
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwTerminate();
	return 0;
}