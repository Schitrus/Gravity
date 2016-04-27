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

const GLuint WIDTH = 1280;
const GLuint HEIGHT = 720;

struct Point
{
	GLdouble mass;
	GLdouble massgain = 0;

	GLdouble density;

	GLdouble radius;

	glm::vec3 position;
	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
};

double wheelpos;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	wheelpos = yoffset;

	//std::cout << yoffset << std::endl;
}

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 2);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Project Gravity", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);


	//Fps cap, 0 = no cap
	glfwSwapInterval(0);

	
	GLfloat ball[3 * 3 * 8] = {
		
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
	
	//Ball

	GLuint VAO_ball;
	glGenVertexArrays(1, &VAO_ball);
	glBindVertexArray(VAO_ball);

	GLuint VBO_ball;
	glGenBuffers(1, &VBO_ball);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(0);



	Shader shader;
	shader.load("vertex.glsl", "fragment.glsl");
	shader.use();

	GLuint Color = glGetUniformLocation(shader.get(), "in_color");



	// Le Physics

	GLfloat stdtime = 0.0f;

	GLfloat deltatime = 0.0f;

	GLfloat frequenstime = 0.0f;

	GLuint numPoints = 190;

	std::vector<Point> objects;

	srand(time(NULL));
	
	//SUN
	objects.push_back(Point());
	objects[0].mass = 1.98855 * pow(10, 30);
	objects[0].position = glm::vec3(
		0.0f, 
		0.0f, 
		0.0f);
	objects[0].radius = 6.957 * pow(10, 8);
	objects[0].velocity = glm::vec2(
		0.0f,
		0.0f);

	//MERCURY
	objects.push_back(Point());
	objects[1].mass = 3.3011 * pow(10, 23);
	objects[1].position = glm::vec3(
		4.60012 * pow(10, 10), 
		0.0f, 
		0.0f);
	objects[1].radius = 2.4397 * pow(10, 6);
	objects[1].velocity = glm::vec2(
		0.0f,
		5.898 * pow(10, 4));

	//VENUS
	objects.push_back(Point());
	objects[2].mass = 4.8675 * pow(10, 24);
	objects[2].position = glm::vec3(
		1.0748 * pow(10, 11),
		0.0f,
		0.0f);
	objects[2].radius = 6.0518 * pow(10, 6);
	objects[2].velocity = glm::vec2(
		0.0f,
		3.526 * pow(10, 4));

	//EARTH
	objects.push_back(Point());
	objects[3].mass = 5.9723 * pow(10, 24);
	objects[3].position = glm::vec3(
		1.4709 * pow(10, 11),
		0.0f,
		0.0f);
	objects[3].radius = 6.378137 * pow(10, 6);
	objects[3].velocity = glm::vec2(
		0.0f,
		3.029 * pow(10, 4));

	//EARTH::MOON
	objects.push_back(Point());
	objects[4].mass = 7.346 * pow(10, 22);
	objects[4].position = glm::vec3(
		1.4709 * pow(10, 11) + 3.633 * pow(10, 8),
		0.0f,
		0.0f);
	objects[4].radius = 1.7381 * pow(10, 6);
	objects[4].velocity = glm::vec2(
		0.0f,
		3.029 * pow(10, 4) + 1.076 * pow(10, 3));

	//MARS
	objects.push_back(Point());
	objects[5].mass = 6.4171 * pow(10, 23);
	objects[5].position = glm::vec3(
		2.0662 * pow(10, 11),
		0.0f,
		0.0f);
	objects[5].radius = 3.3962 * pow(10, 6);
	objects[5].velocity = glm::vec2(
		0.0f,
		2.65 * pow(10, 4));

	//MARS::PHOBOS
	objects.push_back(Point());
	objects[6].mass = 1.06 * pow(10, 16);
	objects[6].position = glm::vec3(
		2.0662 * pow(10, 11) + 9.378 * pow(10, 6),
		0.0f,
		0.0f);
	objects[6].radius = 1.11 * pow(10, 4);
	objects[6].velocity = glm::vec2(
		0.0f,
		2.65 * pow(10, 4) + 2.1385 * pow(10, 3));

	//MARS::DEIMOS
	objects.push_back(Point());
	objects[7].mass = 2.4 * pow(10, 15);
	objects[7].position = glm::vec3(
		2.0662 * pow(10, 11) + 2.3459 * pow(10, 7),
		0.0f,
		0.0f);
	objects[7].radius = 7.8 * pow(10, 3);
	objects[7].velocity = glm::vec2(
		0.0f,
		2.65 * pow(10, 4) + 1.3513 * pow(10, 3));
		
	GLfloat Gravity = 6.6743 * pow(10, -11);

	//Point settings

	GLuint pointSize = 3.0f;

	GLuint lengthSize = 2.0f;

	glEnable(GL_POINT_SPRITE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_POINT_SMOOTH);

	glPointSize(pointSize);

	glLineWidth(lengthSize);

	float speed = 0.07f;
	
	float pausespeed = 0.0f;

	bool pause = false;

	bool oneClick = false;

	GLuint counter = 0;

	GLdouble offset_x = 0.0f;
	GLdouble offset_y = 0.0f;

	GLdouble scale = pow(10, -11);

	GLfloat view_angle = 0.0f;

	enum DisplayColor
	{
		DisplayMass,
		DisplayVelocity,
		DisplayAcceleration,
	};

	bool ColorHit = false;

	int dColor = DisplayMass;


	bool isDead = false;

	glfwSetScrollCallback(window, scroll_callback);

	while (!glfwWindowShouldClose(window)) {


		double xpos_1;
		double ypos_1;

		//std::cout << numPoints << std::endl;

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

			glBindVertexArray(VAO_ball);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

			glUniform4fv(Color, 1, &glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)[0]);

			glClear(GL_COLOR_BUFFER_BIT);

			glDrawArrays(GL_LINES, 0, 2);




			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {





				objects.push_back(Point());
				objects[objects.size() - 1].mass = 0.5f;
				//objects[objects.size() - 1].massgain = 0.0f;
				objects[objects.size() - 1].position.x = (((float)WIDTH / HEIGHT) * (2 * xpos_1 / WIDTH - 1) / scale - offset_x);
				objects[objects.size() - 1].position.y = (1 - (2 * ypos_1 / HEIGHT)) / scale - offset_y;

				objects[objects.size() - 1].velocity.x = (xpos_2 - xpos_1) * deltatime * speed / (10 * objects[objects.size() - 1].mass);
				objects[objects.size() - 1].velocity.y = (ypos_1 - ypos_2) * deltatime * speed / (10 * objects[objects.size() - 1].mass);

				numPoints++;
				oneClick = false;
			}

		}

		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !ColorHit) {
			dColor++;
			if (dColor >= sizeof(DisplayColor))
				dColor = 0;

			ColorHit = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE && ColorHit)
			ColorHit = false;

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
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			view_angle -= frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			view_angle += frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			scale *= 1 + frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			scale *= 1 - frequenstime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		
		if (wheelpos < 0) {
			wheelpos = 1 - (0.03f * (speed+2));
		}
		else if (wheelpos > 0) {
			wheelpos = (0.04f * 1 / (speed+0.25f)) + 1;
		}



		if(wheelpos != 0 && speed > 0)
			speed *= wheelpos;

		//std::cout << speed << "\t" << wheelpos << std::endl;

		wheelpos = 0;

		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && !pause) {
			//std::cout << "pause: " << pause << std::endl;

  			std::swap(speed, pausespeed);
			pause = true;
			
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && pause) {
			pause = false;

		}

		ball[0] =  0.75f;
		ball[1] = -0.75f;

		glPointSize(5);

		glBindVertexArray(VAO_ball);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);

		if (speed == 0)
			glUniform4fv(Color, 1, &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0]);
		else if(speed < 2)
			glUniform4fv(Color, 1, &glm::vec4(0.1f, speed, speed-1, 0.5f)[0]);
		else if (speed < 4)
			glUniform4fv(Color, 1, &glm::vec4(speed-2.9f, 4-speed, 1.0f, 0.5f)[0]);
		else
			glUniform4fv(Color, 1, &glm::vec4(1.0f, 0.0f-4.5f+speed, 1.0f, 0.5f-4+speed)[0]);


		glDrawArrays(GL_POINTS, 0, 1);


		stdtime = glfwGetTime();
		glfwPollEvents();

		//glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		for (int i = 0; i < objects.size(); i++) {


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



			for (int j = 0; j < objects.size(); j++) {
				if (i != j) {


					isDead = false;
					distance_x = (objects[j].position.x - objects[i].position.x);
					distance_y = (objects[j].position.y - objects[i].position.y);


					distance = sqrt(distance_x * distance_x + distance_y * distance_y);

					force = Gravity * (objects[i].mass * objects[j].mass) / (distance * distance);

					force_x += force * distance_x / distance;
					force_y += force * distance_y / distance;
					
					if (distance < (double)(objects[i].radius + objects[j].radius) || ((force / objects[i].mass) * (sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)) + deltatime * speed) * deltatime * speed) > distance){

						std::cout << "distance: " << distance << "\n1. " << (objects[i].radius + objects[j].radius) << "\n2." << ((force / objects[i].mass) * (sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)) + deltatime * speed) * deltatime * speed) << std::endl;

						force_x -= force * distance_x / distance;

						force_y -= force * distance_y / distance;

						int k = i;
						int l = j;
						if (objects[i].mass <= objects[j].mass) {
							std::swap(k, l);

							i--;

							isDead = true;

							j = objects.size() - 1;
						}

						objects[k].velocity.x += objects[l].velocity.x * objects[l].mass / objects[k].mass;
						objects[k].velocity.y += objects[l].velocity.y * objects[l].mass / objects[k].mass;

						objects[k].massgain += objects[l].mass;

						objects[k].position.x -= (objects[l].radius / objects[k].radius) * (objects[k].position.x - objects[l].position.x) / 2;
						objects[k].position.y -= (objects[l].radius / objects[k].radius) * (objects[k].position.y - objects[l].position.y) / 2;


						objects.erase(objects.begin() + l);

						if (!isDead) {
							if (k > l)
								i--;
							j--;
						}
						
					}
					

				}

			}

			if (!isDead) {

				

				objects[i].mass += objects[i].massgain * deltatime * speed * 7;
				objects[i].massgain *= 1 - deltatime * speed * 4;

				if (objects[i].massgain < 0.1f)
					objects[i].massgain = 0;

				acceleration_x = force_x / objects[i].mass;
				acceleration_y = force_y / objects[i].mass;

				objects[i].velocity.x += acceleration_x * 1 * deltatime * speed;
				objects[i].velocity.y += acceleration_y * 1 * deltatime * speed;

				objects[i].position.x += objects[i].velocity.x * deltatime * speed;
				objects[i].position.y += objects[i].velocity.y * deltatime * speed;


				//Övre Höger

				ball[0] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[1] = scale * (objects[i].position.y + offset_y);

				ball[3] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[4] = scale * (objects[i].position.y + offset_y) + 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[6] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[7] = scale * (objects[i].position.y + offset_y);


				ball[9] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[10] = scale * (objects[i].position.y + offset_y) + (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[12] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[13] = scale * (objects[i].position.y + offset_y) + 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[15] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[16] = scale * (objects[i].position.y + offset_y);


				//Övre Vänster

				ball[18] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[19] = scale * (objects[i].position.y + offset_y) + (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[21] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[22] = scale * (objects[i].position.y + offset_y) + 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[24] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[25] = scale * (objects[i].position.y + offset_y);


				ball[27] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[28] = scale * (objects[i].position.y + offset_y);

				ball[30] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[31] = scale * (objects[i].position.y + offset_y) + 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[33] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[34] = scale * (objects[i].position.y + offset_y);


				//Undre Vänster

				ball[36] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[37] = scale * (objects[i].position.y + offset_y);

				ball[39] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[40] = scale * (objects[i].position.y + offset_y) - 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[42] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[43] = scale * (objects[i].position.y + offset_y);


				ball[45] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[46] = scale * (objects[i].position.y + offset_y) - (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[48] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) - 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[49] = scale * (objects[i].position.y + offset_y) - 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[51] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[52] = scale * (objects[i].position.y + offset_y);


				//Undre Höger

				ball[54] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[55] = scale * (objects[i].position.y + offset_y) - (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[57] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[58] = scale * (objects[i].position.y + offset_y) - 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[60] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[61] = scale * (objects[i].position.y + offset_y);


				ball[63] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[64] = scale * (objects[i].position.y + offset_y);

				ball[66] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH) + 0.7f * (scale * objects[i].radius * ((float)HEIGHT / WIDTH) + (3 / (float)WIDTH));
				ball[67] = scale * (objects[i].position.y + offset_y) - 0.7f * (scale * objects[i].radius + (3 / (float)HEIGHT));

				ball[69] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				ball[70] = scale * (objects[i].position.y + offset_y);

				//ball[0] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
				//ball[1] = scale * (objects[i].position.y + offset_y);

				//glPointSize(scale * objects[i].radius + 2.0f);

				glBindVertexArray(VAO_ball);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);


				switch (dColor)
				{
				case DisplayMass:
					glUniform4fv(Color, 1, &glm::vec4(
						0.3f + objects[i].mass * 0.75f,
						0.1f + objects[i].mass * 0.03f,
						0.0f + objects[i].mass * 0.2f - pow(objects[i].mass, 2),
						1.0f
						)[0]);

					break;

				case DisplayVelocity:
					glUniform4fv(Color, 1, &glm::vec4(
						5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
						-0.8f + 5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
						1.0f - 5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
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

		ball[0] = 0.0f;
		ball[1] = 0.0f;

		//ball[0] = 0.0f - objects[0].position.x;
		//ball[1] = 0.0f - objects[0].position.y;

		glPointSize(WIDTH);

		glBindVertexArray(VAO_ball);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);

		if (deltatime > 0.0f)
			glUniform4fv(Color, 1, &glm::vec4(0.0f, 0.0f, 0.0f, 0.05f)[0]);
		else
			glUniform4fv(Color, 1, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glDrawArrays(GL_POINTS, 0, 1);

		glfwSwapBuffers(window);
		//deltatime = glfwGetTime() - stdtime;


		deltatime = 1000.0f;
		frequenstime = glfwGetTime() - stdtime;

		if(!(counter%30))
			std::cout << "FPS:\t" << 1 / frequenstime << "\t\tParticles:\t"<< objects.size() << std::endl;

		counter++;

	}


	std::cout << "Press any key to continue. . ." << std::endl;
	int tmp;
	std::cin >> tmp;
	return 0;

}