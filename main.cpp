#include"Graphics.h"
#include"Shader.h"
#include<chrono>
#include<thread>

using std::cout, std::endl, std::vector, std::array;

bool dropParticles{ false };

//=====================================================================================================================
// Particle Physics ---------------------------------------------------------------------------------------------------
//=====================================================================================================================
ParticleSystem ps;

//=====================================================================================================================
// Variables and Objects declaration-----------------------------------------------------------------------------------
//=====================================================================================================================

bool left_mouse_button;
bool right_mouse_button;
static bool DragFlag{ false };

array<int, 2> windowSize{1000, 1000};
float g_xpos, g_ypos;
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

unsigned int GetClosestParticle(ParticleSystem* ps, Particle& ref)
{
	float closestDistance{ 1000 };
	unsigned int closestID{ 0 };
	glm::vec3 deltap{ glm::vec3(0.0f) };
	float dist{ 0 };

	for (Particle p : ps->Particles)
	{
		if (p.ID == ref.ID)
			continue;

		deltap = p.p - ref.p;
		dist = glm::length2(deltap);

		//cout << "distance from particle " << p.ID << " :" << dist << endl;

		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestID = p.ID;
		}
	}
	//cout << "closest Particle is: particle #" << closestID << endl;
	return closestID;
}


//=====================================================================================================================
// CallBacks ----------------------------------------------------------------------------------------------------------
//=====================================================================================================================
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

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		dropParticles = !dropParticles;
		cout << dropParticles;
	}
}

void mouse_clicked(GLFWwindow* window, int button, int action, int mod)
{
	// Dropping Particles
	if (dropParticles == true)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			glm::vec3 pos{wc_x, wc_y, 0};
			glm::float32 velMag = 10;
			glm::vec3 vel = velMag * glm::cross(glm::normalize(pos), glm::vec3(0.0f, 0.0f, 1.0));

			int lastID = ps.n;

			Particle p(lastID, 0.1f, &pos, &vel);
			ps.AddParticle(&p);
		}
	}

	left_mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
	right_mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

	if (right_mouse_button)
	{
		glm::vec3 pos{ wc_x, wc_y, 0};
		glm::float32 velMag = 10;
		glm::vec3 vel = velMag * glm::cross(glm::normalize(pos), glm::vec3(0.0f, 0.0f, 1.0));
		
		int lastID = ps.n;

		Particle p(lastID, 0.1f, &pos, &vel);
		ps.AddParticle(&p);

		ps.sConstraints.push_back(SpringConstraint(ps.sConstraints.size(),  & ps.Particles[GetClosestParticle(&ps, p)], &ps.Particles[lastID]));

		//ps.SpringParticles.push_back(GetClosestParticle(&ps, p));
		//ps.SpringParticles.push_back(lastID);

		// Add Spring Constants
	}
}

void drag()
{	
	glm::vec3 pos{wc_x, wc_y, 0};
	static Particle DragParticle;

	if (left_mouse_button && DragFlag==false)
	{	
		DragParticle.ID = ps.n;
		DragParticle.p = pos;
		unsigned int closestID = GetClosestParticle(&ps, DragParticle);

		ps.AddParticle(&DragParticle);
		ps.Ignoreparticles.push_back(DragParticle.ID);

		ps.sConstraints.push_back(SpringConstraint(ps.sConstraints.size(), &ps.Particles[DragParticle.ID], &ps.Particles[closestID]));

		//ps.SpringParticles.push_back(DragParticle.ID);
		//ps.SpringParticles.push_back(closestID);

		DragFlag = true;
	}

	else if(left_mouse_button && DragFlag == true)
	{
		ps.Particles[ps.n - 1].p = pos;
	}
	
	else if(!left_mouse_button && DragFlag == true)
	{
		ps.sConstraints.erase(ps.sConstraints.begin() + ps.sConstraints.size() - 1);
		ps.DeleteParticle(DragParticle.ID);
		ps.Ignoreparticles.erase(ps.Ignoreparticles.begin() + ps.Ignoreparticles.size() - 1);
		DragFlag = false;
	}
}

//=====================================================================================================================
// Main ---------------------------------------------------------------------------------------------------------------
//=====================================================================================================================
int main()
{
	using namespace std::this_thread;
	using namespace std::chrono;

	// Creating the Mass Spring Grid
	glm::vec3 pos0, pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9, pos10, pos11, pos12, pos13, pos14, pos15, pos16, pos17, pos18, pos19, pos20, pos21, pos22, pos23;
	pos0 = glm::vec3(-5, 16, 0);
	pos1 = glm::vec3(-3, 16, 0);
	pos2 = glm::vec3(-1, 16, 0);
	pos3 = glm::vec3(1, 16, 0);
	pos4 = glm::vec3(3, 16, 0);
	pos5 = glm::vec3(5, 16, 0);
	pos6 = glm::vec3(-5, 14, 0);
	pos7 = glm::vec3(-3, 14, 0);
	pos8 = glm::vec3(-1, 14, 0);
	pos9 = glm::vec3(1, 14, 0);
	pos10 = glm::vec3(3, 14, 0);
	pos11 = glm::vec3(5, 14, 0);
	pos12 = glm::vec3(-5, 12, 0);
	pos13 = glm::vec3(-3, 11.6, 0);
	pos14 = glm::vec3(-1, 11.4, 0);
	pos15 = glm::vec3(1, 11.4, 0);
	pos16 = glm::vec3(3, 11.6, 0);
	pos17 = glm::vec3(5, 12, 0);
	pos18 = glm::vec3(-5, 10, 0);
	pos19 = glm::vec3(-3, 9.5, 0);
	pos20 = glm::vec3(-1, 9.2, 0);
	pos21 = glm::vec3(1, 9.2, 0);
	pos22 = glm::vec3(3, 9.5, 0);
	pos23 = glm::vec3(5, 10, 0);

	Particle p0(0, 0.94f, &pos0);
	ps.AddParticle(&p0);
	Particle p1(1, 0.94f, &pos1);
	ps.AddParticle(&p1);
	Particle p2(2, 0.94f, &pos2);
	ps.AddParticle(&p2);
	Particle p3(3, 0.94f, &pos3);
	ps.AddParticle(&p3);
	Particle p4(4, 0.94f, &pos4);
	ps.AddParticle(&p4);
	Particle p5(5, 0.94f, &pos5);
	ps.AddParticle(&p5);
	Particle p6(6, 0.94f, &pos6);
	ps.AddParticle(&p6);
	Particle p7(7, 0.94f, &pos7);
	ps.AddParticle(&p7);
	Particle p8(8, 0.94f, &pos8);
	ps.AddParticle(&p8);
	Particle p9(9, 0.94f, &pos9);
	ps.AddParticle(&p9);
	Particle p10(10, 0.94f, &pos10);
	ps.AddParticle(&p10);
	Particle p11(11, 0.94f, &pos11);
	ps.AddParticle(&p11);
	Particle p12(12, 0.94f, &pos12);
	ps.AddParticle(&p12);
	Particle p13(13, 0.94f, &pos13);
	ps.AddParticle(&p13);
	Particle p14(14, 0.94f, &pos14);
	ps.AddParticle(&p14);
	Particle p15(15, 0.94f, &pos15);
	ps.AddParticle(&p15);
	Particle p16(16, 0.94f, &pos16);
	ps.AddParticle(&p16);
	Particle p17(17, 0.94f, &pos17);
	ps.AddParticle(&p17);
	Particle p18(18, 0.94f, &pos18);
	ps.AddParticle(&p18);
	Particle p19(19, 0.94f, &pos19);
	ps.AddParticle(&p19);
	Particle p20(20, 0.94f, &pos20);
	ps.AddParticle(&p20);
	Particle p21(21, 0.94f, &pos21);
	ps.AddParticle(&p21);
	Particle p22(22, 0.94f, &pos22);
	ps.AddParticle(&p22);
	Particle p23(23, 0.94f, &pos23);
	ps.AddParticle(&p23);

	//ps.Ignoreparticles.push_back(0);
	//ps.Ignoreparticles.push_back(1);
	//ps.Ignoreparticles.push_back(2);
	//ps.Ignoreparticles.push_back(3);
	
	ps.sConstraints.push_back(SpringConstraint(0, &ps.Particles[0], &ps.Particles[1]));
	ps.sConstraints.push_back(SpringConstraint(1, &ps.Particles[0], &ps.Particles[6]));
	ps.sConstraints.push_back(SpringConstraint(2, &ps.Particles[0], &ps.Particles[7]));
	ps.sConstraints.push_back(SpringConstraint(3, &ps.Particles[1], &ps.Particles[2]));
	ps.sConstraints.push_back(SpringConstraint(4, &ps.Particles[1], &ps.Particles[7]));
	ps.sConstraints.push_back(SpringConstraint(5, &ps.Particles[1], &ps.Particles[8]));
	ps.sConstraints.push_back(SpringConstraint(6, &ps.Particles[2], &ps.Particles[3]));
	ps.sConstraints.push_back(SpringConstraint(7, &ps.Particles[2], &ps.Particles[8]));
	ps.sConstraints.push_back(SpringConstraint(8, &ps.Particles[2], &ps.Particles[9]));
	ps.sConstraints.push_back(SpringConstraint(9, &ps.Particles[3], &ps.Particles[4]));
	ps.sConstraints.push_back(SpringConstraint(10, &ps.Particles[3], &ps.Particles[9]));
	ps.sConstraints.push_back(SpringConstraint(11, &ps.Particles[3], &ps.Particles[10]));
	ps.sConstraints.push_back(SpringConstraint(12, &ps.Particles[4], &ps.Particles[5]));
	ps.sConstraints.push_back(SpringConstraint(13, &ps.Particles[4], &ps.Particles[10]));
	ps.sConstraints.push_back(SpringConstraint(14, &ps.Particles[4], &ps.Particles[11]));
	ps.sConstraints.push_back(SpringConstraint(15, &ps.Particles[5], &ps.Particles[11]));
	ps.sConstraints.push_back(SpringConstraint(16, &ps.Particles[6], &ps.Particles[1]));
	ps.sConstraints.push_back(SpringConstraint(17, &ps.Particles[6], &ps.Particles[7]));
	ps.sConstraints.push_back(SpringConstraint(18, &ps.Particles[6], &ps.Particles[12]));
	ps.sConstraints.push_back(SpringConstraint(19, &ps.Particles[6], &ps.Particles[13]));
	ps.sConstraints.push_back(SpringConstraint(20, &ps.Particles[7], &ps.Particles[2]));
	ps.sConstraints.push_back(SpringConstraint(21, &ps.Particles[7], &ps.Particles[8]));
	ps.sConstraints.push_back(SpringConstraint(22, &ps.Particles[7], &ps.Particles[13]));
	ps.sConstraints.push_back(SpringConstraint(23, &ps.Particles[7], &ps.Particles[14]));
	ps.sConstraints.push_back(SpringConstraint(24, &ps.Particles[8], &ps.Particles[3]));
	ps.sConstraints.push_back(SpringConstraint(25, &ps.Particles[8], &ps.Particles[9]));
	ps.sConstraints.push_back(SpringConstraint(26, &ps.Particles[8], &ps.Particles[14]));
	ps.sConstraints.push_back(SpringConstraint(27, &ps.Particles[8], &ps.Particles[15]));
	ps.sConstraints.push_back(SpringConstraint(28, &ps.Particles[9], &ps.Particles[4]));
	ps.sConstraints.push_back(SpringConstraint(29, &ps.Particles[9], &ps.Particles[10]));
	ps.sConstraints.push_back(SpringConstraint(30, &ps.Particles[9], &ps.Particles[15]));
	ps.sConstraints.push_back(SpringConstraint(31, &ps.Particles[9], &ps.Particles[16]));
	ps.sConstraints.push_back(SpringConstraint(32, &ps.Particles[10], &ps.Particles[5]));
	ps.sConstraints.push_back(SpringConstraint(33, &ps.Particles[10], &ps.Particles[11]));
	ps.sConstraints.push_back(SpringConstraint(34, &ps.Particles[10], &ps.Particles[16]));
	ps.sConstraints.push_back(SpringConstraint(35, &ps.Particles[10], &ps.Particles[17]));
	ps.sConstraints.push_back(SpringConstraint(36, &ps.Particles[11], &ps.Particles[17]));
	ps.sConstraints.push_back(SpringConstraint(37, &ps.Particles[12], &ps.Particles[7]));
	ps.sConstraints.push_back(SpringConstraint(38, &ps.Particles[12], &ps.Particles[13]));
	ps.sConstraints.push_back(SpringConstraint(39, &ps.Particles[12], &ps.Particles[18]));
	ps.sConstraints.push_back(SpringConstraint(40, &ps.Particles[12], &ps.Particles[19]));
	ps.sConstraints.push_back(SpringConstraint(41, &ps.Particles[13], &ps.Particles[8]));
	ps.sConstraints.push_back(SpringConstraint(42, &ps.Particles[13], &ps.Particles[14]));
	ps.sConstraints.push_back(SpringConstraint(43, &ps.Particles[13], &ps.Particles[19]));
	ps.sConstraints.push_back(SpringConstraint(44, &ps.Particles[13], &ps.Particles[20]));
	ps.sConstraints.push_back(SpringConstraint(45, &ps.Particles[14], &ps.Particles[9]));
	ps.sConstraints.push_back(SpringConstraint(46, &ps.Particles[14], &ps.Particles[15]));
	ps.sConstraints.push_back(SpringConstraint(47, &ps.Particles[14], &ps.Particles[20]));
	ps.sConstraints.push_back(SpringConstraint(48, &ps.Particles[14], &ps.Particles[21]));
	ps.sConstraints.push_back(SpringConstraint(49, &ps.Particles[15], &ps.Particles[10]));
	ps.sConstraints.push_back(SpringConstraint(50, &ps.Particles[15], &ps.Particles[16]));
	ps.sConstraints.push_back(SpringConstraint(51, &ps.Particles[15], &ps.Particles[21]));
	ps.sConstraints.push_back(SpringConstraint(52, &ps.Particles[15], &ps.Particles[22]));
	ps.sConstraints.push_back(SpringConstraint(53, &ps.Particles[16], &ps.Particles[11]));
	ps.sConstraints.push_back(SpringConstraint(54, &ps.Particles[16], &ps.Particles[17]));
	ps.sConstraints.push_back(SpringConstraint(55, &ps.Particles[16], &ps.Particles[22]));
	ps.sConstraints.push_back(SpringConstraint(56, &ps.Particles[16], &ps.Particles[23]));
	ps.sConstraints.push_back(SpringConstraint(57, &ps.Particles[17], &ps.Particles[23]));
	ps.sConstraints.push_back(SpringConstraint(58, &ps.Particles[18], &ps.Particles[13]));
	ps.sConstraints.push_back(SpringConstraint(59, &ps.Particles[18], &ps.Particles[19]));
	ps.sConstraints.push_back(SpringConstraint(60, &ps.Particles[19], &ps.Particles[14]));
	ps.sConstraints.push_back(SpringConstraint(61, &ps.Particles[19], &ps.Particles[20]));
	ps.sConstraints.push_back(SpringConstraint(62, &ps.Particles[20], &ps.Particles[15]));
	ps.sConstraints.push_back(SpringConstraint(63, &ps.Particles[20], &ps.Particles[21]));
	ps.sConstraints.push_back(SpringConstraint(64, &ps.Particles[21], &ps.Particles[16]));
	ps.sConstraints.push_back(SpringConstraint(65, &ps.Particles[21], &ps.Particles[22]));
	ps.sConstraints.push_back(SpringConstraint(66, &ps.Particles[22], &ps.Particles[17]));
	ps.sConstraints.push_back(SpringConstraint(67, &ps.Particles[22], &ps.Particles[23]));

	/*ps.SpringParticles.push_back(0);
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

	ps.SpringParticles.push_back(2);
	ps.SpringParticles.push_back(4);

	ps.SpringParticles.push_back(2);
	ps.SpringParticles.push_back(5);

	ps.SpringParticles.push_back(3);
	ps.SpringParticles.push_back(4);

	ps.SpringParticles.push_back(3);
	ps.SpringParticles.push_back(5);

	ps.SpringParticles.push_back(4);
	ps.SpringParticles.push_back(5);*/

	//=================================================================================================================
	// GLFW initialization --------------------------------------------------------------------------------------------
	//=================================================================================================================
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

	//=================================================================================================================
	// CallBacks ------------------------------------------------------------------------------------------------------
	//=================================================================================================================
	// updating viewport size if window size is changed CallBack
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callBack);
	glfwSetMouseButtonCallback(window, mouse_clicked);
	glfwSetKeyCallback(window, key_callback);

	//=================================================================================================================
	// Shader Compilation ---------------------------------------------------------------------------------------------
	//=================================================================================================================
	Shader SpringShader;
	Shader MainShader;
	
	//=================================================================================================================
	// Graphical Objects Declaration ----------------------------------------------------------------------------------
	//=================================================================================================================
	GraphicalObj rectangle(MainShader, "./Textures/GlowDotFilled.png");
	GraphicalObj SpringShape(SpringShader, "./Textures/Spring.png");
	Colors color;

	bool start_flag{ true };

	//=================================================================================================================
	// Program Loop ---------------------------------------------------------------------------------------------------
	//=================================================================================================================
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

		//if (DeltaT > 0.05)
		{
			previousTime = runtime;

			// Dragging the Particles
			if (dropParticles == false)
			{
				drag();
			}

			// Solve
			ps.EulerSolve(float(DeltaT));


			// Render
			if (!ps.Particles.empty())
			{
				rectangle.getShader().use();
				for (int i = 0; i < ps.n; ++i)
				{
					/*if (i == 0 || i==1)
					{
						rectangle.transform(glm::vec3(Scale_x, Scale_y, 1.0f), glm::vec3(ps.Particles[i].p[0], ps.Particles[i].p[1], 0.0f), 30, glm::vec3(1.0f));
						rectangle.DrawShape(color.Amber);

					}*/

					//else
					{
						rectangle.transform(glm::vec3(Scale_x, Scale_y, 1.0f), glm::vec3(ps.Particles[i].p[0], ps.Particles[i].p[1], 0.0f));
						rectangle.DrawShape(color.White);
					}
				}

				if (!ps.sConstraints.empty())
				{
					SpringShape.getShader().use();
					for (int i{ 0 }; i < ps.sConstraints.size(); ++i)
					{

						glm::vec3 deltap = ps.Particles[ps.sConstraints[i].getParticleIDs()[1]].p - ps.Particles[ps.sConstraints[i].getParticleIDs()[0]].p;
						SpringShape.transform(
							glm::vec3(Scale_x, Scale_y, 1.0f),
							glm::vec3(
								(ps.Particles[ps.sConstraints[i].getParticleIDs()[0]].p[0] + ps.Particles[ps.sConstraints[i].getParticleIDs()[1]].p[0]) / 2.0f,
								(ps.Particles[ps.sConstraints[i].getParticleIDs()[0]].p[1] + ps.Particles[ps.sConstraints[i].getParticleIDs()[1]].p[1]) / 2.0f,
								0.0f),
							-glm::atan(deltap[0]/deltap[1]));

						SpringShape.DrawShape(color.White);
					}
				}
			}
		}

		if (start_flag)
		{
			sleep_for(seconds(2));
			start_flag = false;
		}
		// Print FPS
		//if (timer > 500)
		//{
		//	//system("CLS");
		//	cout << "FPS: " << 1 / DeltaT << endl;
		//	timer = 0;
		//}
		//++timer;
	}

	// Unbinding and closing all glfw windows and clearing opbjects
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwTerminate();
	return 0;
}