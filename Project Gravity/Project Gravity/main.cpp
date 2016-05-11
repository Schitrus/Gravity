#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm\glm.hpp>

//#include <iostream>
//#include <cstdlib>
#include <ctime>

//#include <algorithm>

#include <vector>

#include "Shader.h"

GLFWwindow* window;

GLuint WIDTH = 1377;
GLuint HEIGHT = 768;

struct Point
{
	GLdouble mass;

	GLdouble density;

	GLdouble radius;

	glm::vec3 position;
	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

double wheelpos;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	wheelpos = yoffset;

	//std::cout << yoffset << std::endl;
}

int main() {


	//Introduction
	std::cout << "#####PROJECT GRAVITY#####\n\nA Highschoolwork by Karl Andersson\n\n\n";


	//Window dimensions
	std::cout << "Enter the windows:\nWidth, ";
	std::cin >> WIDTH;
	std::cout << "Height, ";
	std::cin >> HEIGHT;

	//Choose Scenario

	enum Scenario{
		Solarsystem = 1,
		Homogeneity,
		Random,
		Massive,
		Orbits
	};

	int scenario = 0;

	std::cout << "##### Scenarios #####\n\n";
	std::cout << "1.\tSolarsystem\n2.\tHomogeneity\n3.\tRandom\n4.\tMassive\n5.\tOrbits";
	for (int i = 0; i < 24 - 6; i++) std::cout << "\n";

	std::cin >> scenario;

	if (scenario <= 0 || scenario >  5)
		scenario = 1;


	//Initialize GLFW and Window
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 2);

	if (WIDTH > 1920 || HEIGHT > 1080 || WIDTH == 1 || HEIGHT == 1) {
		WIDTH = 1920; HEIGHT = 1080;
		window = glfwCreateWindow(WIDTH, HEIGHT, "Project Gravity", glfwGetPrimaryMonitor(), nullptr);
	}
	else if (WIDTH < 128 || HEIGHT < 128) {
		WIDTH = 1377; HEIGHT = 768;
		window = glfwCreateWindow(WIDTH, HEIGHT, "Project Gravity", nullptr, nullptr);
	}
	else
	window = glfwCreateWindow(WIDTH, HEIGHT, "Project Gravity", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glfwSetScrollCallback(window, scroll_callback);


	//Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WIDTH, HEIGHT);


	//Fps cap, 0 = no cap
	glfwSwapInterval(0);

	//Octogon shape for particles made out of 8 triangles
	GLfloat octogon[3 * 3 * 8] = {
		
		 1.0f, 0.0f, 0.0f,
		 0.7f, 0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,

		 0.0f, 1.0f, 0.0f,
		 0.7f, 0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,


		 0.0f, 1.0f, 0.0f,
		-0.7f, 0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-0.7f, 0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,


		-1.0f, 0.0f, 0.0f,
		-0.7f,-0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,

		 0.0f,-1.0f, 0.0f,
		-0.7f,-0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,


		 0.0f,-1.0f, 0.0f,
		 0.7f,-0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,

		 1.0f, 0.0f, 0.0f,
		 0.7f,-0.7f, 0.0f,
		 0.0f, 0.0f, 0.0f,

	};

	//Vertex Array Object
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	//Vertex Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(octogon), octogon, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	//Load Shaders vertex.glsl, fragment.glsl
	Shader shader;
	shader.load("vertex.glsl", "fragment.glsl");
	shader.use();

	GLuint Color = glGetUniformLocation(shader.get(), "in_color");

	////////////////////////////////
	///// VARIABLES
	/////////////////

	GLfloat stdtime = 0.0f;
	GLfloat frequenstime = 0.0f;
	
	GLboolean constanttime;

	GLfloat deltaspeed;
	GLfloat deltatime = 0;
	GLdouble scale;

	GLdouble offset_x = 0.0f;
	GLdouble offset_y = 0.0f;

	enum DisplayColor
	{
		DisplayVisible,
		DisplayMass,
		DisplayVelocity,
		DisplayAcceleration,
	};

	int dColor = DisplayVisible;

	float speed = 1.0f;
	float pausespeed = 0.0f;
	bool pause = false;

	bool oneClick = false;
	bool ColorHit = false;
	bool isDead = false;

	GLuint counter = 0;

	//The Gravity Constant
	GLfloat Gravity = 6.6743 * pow(10, -11);

	//////////////////////////////////
	///// SCENARIOS
	//////////////////////////

	//Create vector of particles
	std::vector<Point> particle;

	switch (scenario) {
	
	case Solarsystem:

		constanttime = true;

		deltaspeed = 100000.0f;

		scale = pow(10, -11);

		//SUN
		particle.push_back(Point());
		particle[0].mass = 1.98855 * pow(10, 30);
		particle[0].position = glm::vec3(
			0.0f,
			0.0f,
			0.0f);
		particle[0].radius = 6.957 * pow(10, 8);
		particle[0].velocity = glm::vec2(
			0.0f,
			0.0f);
		particle[0].color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		particle[0].density = particle[0].mass / (3.14f * pow(particle[0].radius, 2));

		//MERCURY
		particle.push_back(Point());
		particle[1].mass = 3.3011 * pow(10, 23);
		particle[1].position = glm::vec3(
			4.60012 * pow(10, 10),
			0.0f,
			0.0f);
		particle[1].radius = 2.4397 * pow(10, 6);
		particle[1].velocity = glm::vec2(
			0.0f,
			5.898 * pow(10, 4));
		particle[1].color = glm::vec4(0.6f, 0.55f, 0.45f, 1.0f);
		particle[1].density = particle[1].mass / (3.14f * pow(particle[1].radius, 2));

		//VENUS
		particle.push_back(Point());
		particle[2].mass = 4.8675 * pow(10, 24);
		particle[2].position = glm::vec3(
			1.0748 * pow(10, 11),
			0.0f,
			0.0f);
		particle[2].radius = 6.0518 * pow(10, 6);
		particle[2].velocity = glm::vec2(
			0.0f,
			3.526 * pow(10, 4));
		particle[2].color = glm::vec4(0.85f, 0.81f, 0.48f, 1.0f);
		particle[2].density = particle[2].mass / (3.14f * pow(particle[2].radius, 2));

		//EARTH
		particle.push_back(Point());
		particle[3].mass = 5.9723 * pow(10, 24);
		particle[3].position = glm::vec3(
			1.4709 * pow(10, 11),
			0.0f,
			0.0f);
		particle[3].radius = 6.378137 * pow(10, 6);
		particle[3].velocity = glm::vec2(
			0.0f,
			3.029 * pow(10, 4));
		particle[3].color = glm::vec4(0.28f, 0.46f, 0.77f, 1.0f);
		particle[3].density = particle[3].mass / (3.14f * pow(particle[3].radius, 2));

		//EARTH::MOON
		particle.push_back(Point());
		particle[4].mass = 7.346 * pow(10, 22);
		particle[4].position = glm::vec3(
			1.4709 * pow(10, 11) + 3.633 * pow(10, 8),
			0.0f,
			0.0f);
		particle[4].radius = 1.7381 * pow(10, 6);
		particle[4].velocity = glm::vec2(
			0.0f,
			3.029 * pow(10, 4) + 1.076 * pow(10, 3));
		particle[4].color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		particle[4].density = particle[4].mass / (3.14f * pow(particle[4].radius, 2));

		//MARS
		particle.push_back(Point());
		particle[5].mass = 6.4171 * pow(10, 23);
		particle[5].position = glm::vec3(
			2.0662 * pow(10, 11),
			0.0f,
			0.0f);
		particle[5].radius = 3.3962 * pow(10, 6);
		particle[5].velocity = glm::vec2(
			0.0f,
			2.65 * pow(10, 4));
		particle[5].color = glm::vec4(0.75f, 0.39f, 0.27f, 1.0f);
		particle[5].density = particle[5].mass / (3.14f * pow(particle[5].radius, 2));

		//Jupiter
		particle.push_back(Point());
		particle[6].mass = 1.89819 * pow(10, 27);
		particle[6].position = glm::vec3(
			7.4052 * pow(10, 11),
			0.0f,
			0.0f);
		particle[6].radius = 7.1492 * pow(10, 7);
		particle[6].velocity = glm::vec2(
			0.0f,
			1.372 * pow(10, 4));
		particle[6].color = glm::vec4(0.72f, 0.52f, 0.18f, 1.0f);
		particle[6].density = particle[6].mass / (3.14f * pow(particle[6].radius, 2));

		//Saturn
		particle.push_back(Point());
		particle[7].mass = 5.6834 * pow(10, 26);
		particle[7].position = glm::vec3(
			1.35255 * pow(10, 12),
			0.0f,
			0.0f);
		particle[7].radius = 6.0268 * pow(10, 7);
		particle[7].velocity = glm::vec2(
			0.0f,
			1.018 * pow(10, 4));
		particle[7].color = glm::vec4(0.88f, 0.86f, 0.39f, 1.0f);
		particle[7].density = particle[7].mass / (3.14f * pow(particle[7].radius, 2));

		//Uranus
		particle.push_back(Point());
		particle[8].mass = 8.6813 * pow(10, 25);
		particle[8].position = glm::vec3(
			2.7413 * pow(10, 12),
			0.0f,
			0.0f);
		particle[8].radius = 2.5559 * pow(10, 7);
		particle[8].velocity = glm::vec2(
			0.0f,
			7.11 * pow(10, 3));
		particle[8].color = glm::vec4(0.57f, 0.84f, 0.95f, 1.0f);
		particle[8].density = particle[8].mass / (3.14f * pow(particle[8].radius, 2));

		//Neptune
		particle.push_back(Point());
		particle[9].mass = 1.02413 * pow(10, 26);
		particle[9].position = glm::vec3(
			4.44445 * pow(10, 12),
			0.0f,
			0.0f);
		particle[9].radius = 2.4764 * pow(10, 7);
		particle[9].velocity = glm::vec2(
			0.0f,
			5.5 * pow(10, 3));
		particle[9].color = glm::vec4(0.07f, 0.29f, 0.80f, 1.0f);
		particle[9].density = particle[9].mass / (3.14f * pow(particle[9].radius, 2));

		break;


	case Homogeneity:

		deltaspeed = 100.0f;

		constanttime = true;

		scale = 0.9f;
		
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				particle.push_back(Point());
				particle[i * 9 + j].mass = 1.0f;
				particle[i * 9 + j].position = glm::vec3(1.0f - (0.25f)*i, 1.0f - (0.25f)*j, 1.0f);
				particle[i * 9 + j].radius = 0.00001f;
				particle[i * 9 + j].density = particle[i * 9 + j].mass / (3.14f * pow(particle[i * 9 + j].radius, 2));
			}
		}
		
		/*
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				particle.push_back(Point());
				particle[i * 9 + j].mass = 1.0f;
				if (!(j % 2)) {
					particle[i * 9 + j].position = glm::vec3(1.0f - (0.25f)*i, 1.0f - (0.2f)*j, 1.0f);
				}
				else {
					particle[i * 9 + j].position = glm::vec3(1.125f - (0.25f)*i, 1.0f - (0.2f)*j, 1.0f);
				}
				particle[i * 9 + j].radius = 0.025f;
				particle[i * 9 + j].density = particle[i * 9 + j].mass / (3.14f * pow(particle[i * 9 + j].radius, 2));
			}
		}
		*/
		
		break;

	case Random:

		deltaspeed = 1000.0f;

		constanttime = true;

		scale = 0.2f;

		srand(time(NULL));

		for (int i = 0; i < 175; i++) {
			particle.push_back(Point());
			particle[i].mass = (float)(rand() % 100 + 1)/200;
			particle[i].position = glm::vec3((float)(rand() % 4000) / 500 - 4, (float)(rand() % 4000) / 500 - 4, 1.0f);
			particle[i].radius = (float)(rand() % 100 + 1)/25 * particle[i].mass * 0.01f;
			particle[i].color = glm::vec4((float)(rand() % 100) / 100, (float)(rand() % 100) / 100, (float)(rand() % 100) / 100, 1.0f);
			particle[i].density = particle[i].mass / (3.14f * pow(particle[i].radius, 2));

		}

		break;

	case Massive:

		deltaspeed = 300.0f;

		constanttime = true;

		scale = 0.1f;

		srand(time(NULL));

		for (int i = 0; i < 200; i++) {
			particle.push_back(Point());
			particle[i].mass = (float)(rand() % 100 + 1) / 200;
			particle[i].position = glm::vec3((float)(rand() % 10000) / 500 - 10, (float)(rand() % 10000) / 500 - 10, 1.0f);
			particle[i].radius = (float)(rand() % 100 + 1) / 25 * particle[i].mass * 0.01f;
			particle[i].color = glm::vec4((float)(rand() % 100) / 100, (float)(rand() % 100) / 100, (float)(rand() % 100) / 100, 1.0f);
			particle[i].velocity.x = 0.00005f * ((float)(rand()%100)/50 - 1);
			particle[i].velocity.y = 0.00005f * ((float)(rand() % 100) / 50 - 1);
			particle[i].density = particle[i].mass / (3.14f * pow(particle[i].radius, 2));

		}

		particle.push_back(Point());
		particle[particle.size() - 1].mass = 400.0f;
		particle[particle.size() - 1].position = glm::vec3(0.0f, 0.0f, 1.0f);
		particle[particle.size() - 1].radius = 0.00001f;
		particle[particle.size() - 1].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particle[particle.size() - 1].density = particle[particle.size() - 1].mass / (3.14f * pow(particle[particle.size() - 1].radius, 2));

		break;

	case Orbits:

		deltaspeed = 1.0f;

		constanttime = true;

		scale = 8.0f;
		particle.push_back(Point());
		particle[0].mass = 2000.0f;
		particle[0].position = glm::vec3(0.0f, 0.0f, 1.0f);
		particle[0].radius = 0.00001f;
		//particle[particle.size() - 1].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particle[0].density = particle[0].mass / (3.14f * pow(particle[0].radius, 2));

		for (int i = 1; i <= 100; i++) {
			particle.push_back(Point());
			particle[i].mass = 0.001f;
			particle[i].position = glm::vec3(0.0f, pow(-1, i) * (0.01f + 0.001f * i), 1.0f);
			particle[i].radius = 0.00001f;
			//particle[particle.size() - 1].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			particle[i].density = particle[i].mass / (3.14f * pow(particle[i].radius, 2));
			particle[i].velocity = glm::vec2(pow(-1, i) * Gravity * sqrt(2) * pow(particle[0].mass, 2) / sqrt(abs(particle[i].position.y)), 0.0f);
		}

		break;

	default:

		deltaspeed = 1.0f;

		constanttime = true;

		scale = 1.0f;

		particle.push_back(Point());
		particle[0].mass = 1.0f;
		particle[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
		particle[0].radius = 1.0f;
		particle[0].velocity = glm::vec2(0.0f, 0.0f);
		particle[0].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particle[0].density = particle[0].mass / (3.14f * pow(particle[0].radius, 2));

		break;
	};

	//Draw settings

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint pointSize = 3.0f;
	GLuint lengthSize = 2.0f;

	glPointSize(pointSize);
	glLineWidth(lengthSize);

	////////////////////////////////////////
	/////// GAME LOOOP
	////////////////////////////////////

	while (!glfwWindowShouldClose(window)) {

		//Update stdtime
		stdtime = glfwGetTime();
		glfwPollEvents();


		// Close program
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		double xpos_1;
		double ypos_1;

		////////////////////////////
		////Left Mouse Button add particle
		/////////////////
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || oneClick) {

			if (!oneClick)
				glfwGetCursorPos(window, &xpos_1, &ypos_1);

			oneClick = true;

			double xpos_2;
			double ypos_2;

			glfwGetCursorPos(window, &xpos_2, &ypos_2);

			GLfloat line[6] = {
				2 * xpos_1 / WIDTH - 1, 1 - 2 * ypos_1 / HEIGHT , 1.0f,
				2 * xpos_2 / WIDTH - 1, 1 - 2 * ypos_2 / HEIGHT, 1.0f
			};

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

			glUniform4fv(Color, 1, &glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)[0]);

			glClear(GL_COLOR_BUFFER_BIT);

			glDrawArrays(GL_LINES, 0, 2);

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {

				particle.push_back(Point());
				particle[particle.size() - 1].mass = 0.5f;
				particle[particle.size() - 1].position.x = (((float)WIDTH / HEIGHT) * (2 * xpos_1 / WIDTH - 1) / scale - offset_x);
				particle[particle.size() - 1].position.y = (1 - (2 * ypos_1 / HEIGHT)) / scale - offset_y;

				particle[particle.size() - 1].velocity.x = (xpos_2 - xpos_1) * deltatime * speed / (10 * particle[particle.size() - 1].mass);
				particle[particle.size() - 1].velocity.y = (ypos_1 - ypos_2) * deltatime * speed / (10 * particle[particle.size() - 1].mass);

				oneClick = false;
			}

		}

		/////////////////////
		///// Color Mode
		//////////
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !ColorHit) {
			dColor++;
			if (dColor >= sizeof(DisplayColor))
				dColor = 0;

			ColorHit = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE && ColorHit)
			ColorHit = false;


		////////////////////
		//// Move Screen
		//////////////
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			offset_x += frequenstime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			offset_x -= frequenstime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			offset_y -= frequenstime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			offset_y += frequenstime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}

		/////////////////
		//// ZOOM
		////////////
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			scale *= 1 + frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			scale *= 1 - frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		

		///////////////////
		//// Speed
		/////////////////
		if (wheelpos < 0) {
			wheelpos = 1 - (0.03f * (speed+2));
		}
		else if (wheelpos > 0) {
			wheelpos = (0.04f * 1 / (speed+0.25f)) + 1;
		}

		if(wheelpos != 0 && speed > 0)
			speed *= wheelpos;

		wheelpos = 0;


		////////////////////
		//// Pause
		//////////////////
		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && !pause) {

  			std::swap(speed, pausespeed);
			pause = true;
			
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && pause) {
			pause = false;

		}

		///////////////////
		//// Speed indicator
		////////////////
		octogon[0] =  0.75f;
		octogon[1] = -0.75f;

		glPointSize(5);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(octogon), octogon, GL_STATIC_DRAW);

		if (speed == 0)
			glUniform4fv(Color, 1, &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0]);
		else if(speed < 2)
			glUniform4fv(Color, 1, &glm::vec4(0.1f, speed, speed-1, 0.5f)[0]);
		else if (speed < 4)
			glUniform4fv(Color, 1, &glm::vec4(speed-2.9f, 4-speed, 1.0f, 0.5f)[0]);
		else
			glUniform4fv(Color, 1, &glm::vec4(1.0f, 0.0f-4.5f+speed, 1.0f, 0.5f-4+speed)[0]);

		glDrawArrays(GL_POINTS, 0, 1);

		///////////////////////////////
		////// Calculate particles
		////////////////////////////////
		for (int i = 0; i < particle.size(); i++) {


			GLfloat acceleration_x, acceleration_y;
			acceleration_x = 0.0f;
			acceleration_y = 0.0f;

			GLfloat distance;
			GLfloat distance_x;
			GLfloat distance_y;

			GLfloat force;
			GLfloat force_x, force_y;

			force = 0.0f;

			force_x = 0.0f;
			force_y = 0.0f;


			///////////////////////////////
			/////// Compare particles
			////////////////////////////////
			for (int j = 0; j < particle.size(); j++) {
				if (i != j) {

					isDead = false;
					distance_x = (particle[j].position.x - particle[i].position.x);
					distance_y = (particle[j].position.y - particle[i].position.y);


					distance = sqrt(distance_x * distance_x + distance_y * distance_y);

					force = Gravity * (particle[i].mass * particle[j].mass) / (distance * distance);

					force_x += force * distance_x / distance;
					force_y += force * distance_y / distance;

					////////////////////////////////
					////// Merge Partcles
					///////////////////////
					if (distance < (double)(particle[i].radius + particle[j].radius) || ((force / particle[i].mass) * (sqrt(pow(particle[i].velocity.x, 2) + pow(particle[i].velocity.y, 2)) + deltatime * speed) * deltatime * speed) > distance){

						force_x -= force * distance_x / distance;
						force_y -= force * distance_y / distance;

						int k = i;
						int l = j;
						if (particle[i].mass <= particle[j].mass) {
							std::swap(k, l);

							i--;

							isDead = true;

							j = particle.size() - 1;
						}

						if (particle[l].radius < particle[k].radius) {
							particle[k].position.x -= (particle[l].radius / particle[k].radius) * (particle[k].position.x - particle[l].position.x) / 2;
							particle[k].position.y -= (particle[l].radius / particle[k].radius) * (particle[k].position.y - particle[l].position.y) / 2;
						}
						else {
							particle[k].position.x -= (particle[k].radius / particle[l].radius) * (particle[k].position.x - particle[l].position.x) / 2;
							particle[k].position.y -= (particle[k].radius / particle[l].radius) * (particle[k].position.y - particle[l].position.y) / 2;
						}
						particle[k].velocity.x += particle[l].velocity.x * particle[l].mass / particle[k].mass;
						particle[k].velocity.y += particle[l].velocity.y * particle[l].mass / particle[k].mass;

						//particle[k].radius += sqrt(pow(particle[l].radius, 2) * 3.14f) / 3.14f;
						particle[k].mass += particle[l].mass;
						particle[k].radius = sqrt((particle[k].mass / particle[k].density) / 3.14f);




						particle.erase(particle.begin() + l);

						if (!isDead) {
							if (k > l)
								i--;
							j--;
						}
						
					}
					

				}

			}

			if (!isDead) {
				///////////////////////
				//// Update position
				////////////////////////
				acceleration_x = force_x / particle[i].mass;
				acceleration_y = force_y / particle[i].mass;

				particle[i].velocity.x += acceleration_x * 1 * deltatime * speed;
				particle[i].velocity.y += acceleration_y * 1 * deltatime * speed;

				particle[i].position.x += particle[i].velocity.x * deltatime * speed;
				particle[i].position.y += particle[i].velocity.y * deltatime * speed;

				//////Follow particle

				//if (true) {
				//	offset_x = -particle[particle.size() - 1].position.x;
				//	offset_y = -particle[particle.size() - 1].position.y;
				//}

				/////////Upper Right

				octogon[0] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[1] = scale * (particle[i].position.y + offset_y);

				octogon[3] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[4] = scale * (particle[i].position.y + offset_y) + 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[6] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[7] = scale * (particle[i].position.y + offset_y);


				octogon[9] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[10] = scale * (particle[i].position.y + offset_y) + (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[12] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[13] = scale * (particle[i].position.y + offset_y) + 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[15] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[16] = scale * (particle[i].position.y + offset_y);


				///////////Upper Left

				octogon[18] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[19] = scale * (particle[i].position.y + offset_y) + (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[21] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[22] = scale * (particle[i].position.y + offset_y) + 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[24] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[25] = scale * (particle[i].position.y + offset_y);


				octogon[27] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[28] = scale * (particle[i].position.y + offset_y);

				octogon[30] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[31] = scale * (particle[i].position.y + offset_y) + 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[33] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[34] = scale * (particle[i].position.y + offset_y);


				///////////Lower Left

				octogon[36] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[37] = scale * (particle[i].position.y + offset_y);

				octogon[39] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[40] = scale * (particle[i].position.y + offset_y) - 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[42] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[43] = scale * (particle[i].position.y + offset_y);


				octogon[45] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[46] = scale * (particle[i].position.y + offset_y) - (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[48] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[49] = scale * (particle[i].position.y + offset_y) - 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[51] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[52] = scale * (particle[i].position.y + offset_y);


				//////////Lower Right

				octogon[54] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[55] = scale * (particle[i].position.y + offset_y) - (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[57] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[58] = scale * (particle[i].position.y + offset_y) - 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[60] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[61] = scale * (particle[i].position.y + offset_y);


				octogon[63] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[64] = scale * (particle[i].position.y + offset_y);

				octogon[66] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * particle[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				octogon[67] = scale * (particle[i].position.y + offset_y) - 0.7f * (scale * particle[i].radius + (3 / (float)HEIGHT));

				octogon[69] = scale * (particle[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				octogon[70] = scale * (particle[i].position.y + offset_y);

				//////////////////////////////////
				///////// Draw Particles
				//////////////////////
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(octogon), octogon, GL_STATIC_DRAW);


				switch (dColor)
				{
				case DisplayVisible:
					glUniform4fv(Color, 1, &particle[i].color[0]);

					break;

				case DisplayMass:
					glUniform4fv(Color, 1, &glm::vec4(
						0.3f + particle[i].mass * 0.75f,
						0.1f + particle[i].mass * 0.03f,
						0.0f + particle[i].mass * 0.2f - pow(particle[i].mass, 2),
						1.0f
						)[0]);

					break;

				case DisplayVelocity:
					glUniform4fv(Color, 1, &glm::vec4(
						5 * sqrt(pow(particle[i].velocity.x, 2) + pow(particle[i].velocity.y, 2)),
						-0.8f + 5 * sqrt(pow(particle[i].velocity.x, 2) + pow(particle[i].velocity.y, 2)),
						1.0f - 5 * sqrt(pow(particle[i].velocity.x, 2) + pow(particle[i].velocity.y, 2)),
						1.0f
						)[0]);

					break;

				case DisplayAcceleration:
					glUniform4fv(Color, 1, &glm::vec4(
						0.5f + sqrt(pow(acceleration_x, 2) + pow(acceleration_y, 2)),
						0.1f + 20 * sqrt(pow(acceleration_x, 2) + pow(acceleration_y, 2)),
						0.5f + 0.5f * sqrt(pow(acceleration_x, 2) + pow(acceleration_y, 2)),
						1.0f
						)[0]);

					break;

				default:

					glUniform4fv(Color, 1, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);

					break;
				}


				glDrawArrays(GL_TRIANGLES, 0, 3 * 8);

			}
		}

		////////////////////////////
		/////// Draw trail
		/////////////////
		octogon[0] = 0.0f;
		octogon[1] = 0.0f;

		glPointSize(WIDTH);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(octogon), octogon, GL_STATIC_DRAW);

		if (counter > 0)
			glUniform4fv(Color, 1, &glm::vec4(0.0f, 0.0f, 0.0f, 0.04f)[0]);
		else
			glUniform4fv(Color, 1, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glDrawArrays(GL_POINTS, 0, 1);

		///////////////////////////
		///// Update deltatime
		////////////////////////

		counter++;

		glfwSwapBuffers(window);
		frequenstime = glfwGetTime() - stdtime;

		if (constanttime)
			deltatime = deltaspeed * frequenstime;
		else
			deltatime = deltaspeed;

		//if(!(counter % (int)(1 / (frequenstime + pow(10, -1)))))
		//std::cout << "FPS:\t" << 1 / frequenstime << "\t\tParticles:\t"<< particle.size() << "\t\tSpeed:\t" << speed << std::endl;

		counter++;
	}
	//close window
	glfwTerminate();


	//wait for input to close cmd
	std::cout << "Press any key to continue. . ." << std::endl;
	int tmp;
	std::cin >> tmp;
	return 0;

}