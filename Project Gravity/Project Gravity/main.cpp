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
	GLfloat mass;
	GLfloat massgain = 0;
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
	glfwWindowHint(GLFW_SAMPLES, 0);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Project Gravity", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);


	GLfloat ball[3] = {
		0.0f, 0.0f, 0.0f
	};


	/*
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
	*/
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

	GLuint numPoints = 100;

	std::vector<Point> objects;

	srand(time(NULL));

	for (int i = 0; i < numPoints; i++) {

		objects.push_back(Point());

		objects[i].mass = 0.1f;
		//objects[i].mass = ((float)(rand() % 2000) / 1000) + 0.01f;
		objects[i].position = glm::vec3(((float)(rand() % 200) / (100 / ((float)WIDTH / HEIGHT)) - ((float)WIDTH / HEIGHT)), ((float)(rand() % 200) / 100 - 1), 1.0f);
		//objects[i].velocity = glm::vec2(((float)(rand() % 2000) / 10000) - 0.1f, ((float)(rand() % 2000) / 10000) - 0.1f);

	}




	GLfloat Gravity = 6.67 * pow(10, -5);
	/*
	objects[0].mass = 42.0f;
	objects[0].position = glm::vec3(0.0f, 0.0f, 1.0f);

	objects[1].mass = 0.1f;
	objects[1].position = glm::vec3(0.2f, 0.0f, 1.0f);
	objects[1].velocity.y = sqrt(2 * Gravity * objects[0].mass / 0.2f);

	objects[2].mass = 0.001f;
	objects[2].position = glm::vec3(0.25f, 0.0f, 1.0f);
	objects[2].velocity.y = sqrt(2 * Gravity * objects[0].mass / 0.25f) + sqrt(2 * Gravity * objects[1].mass / 0.05f);

	objects[3].mass = 0.8f;
	objects[3].position = glm::vec3(0.5f, 0.0f, 1.0f);
	objects[3].velocity.y = sqrt(2 * Gravity * objects[0].mass / 0.7f);
		
	objects[4].mass = 0.02f;
	objects[4].position = glm::vec3(0.55f, 0.0f, 1.0f);
	objects[4].velocity.y = sqrt(2 * Gravity * objects[0].mass / 0.40f);

	objects[5].mass = 0.01f;
	objects[5].position = glm::vec3(0.0f,-0.85f, 1.0f);
	objects[5].velocity.x = -sqrt(2 * Gravity * objects[0].mass / 0.86f);

	objects[6].mass = 0.03f;
	objects[6].position = glm::vec3(0.03f,-0.47f, 1.0f);
	objects[6].velocity.x = sqrt(2 * Gravity * objects[0].mass / 0.47f);

	objects[7].mass = 0.05f;
	objects[7].position = glm::vec3(-0.09f,-0.61f, 1.0f);
	objects[7].velocity.x = -sqrt(2 * Gravity * objects[0].mass / 0.65f);
	*/
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

	float speed = 0.1f;
	
	float pausespeed = 0.0f;

	bool pause = false;

	//glColorMask(1, 1, 1, 0);

	bool oneClick = false;

	GLuint counter = 0;

	GLfloat offset_x = 0.0f;
	GLfloat offset_y = 0.0f;

	GLfloat scale = 1.0f;

	GLfloat view_angle = 0.0f;


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

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			offset_x += 2 * deltatime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			offset_x -= 2 * deltatime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			offset_y -= 2 * deltatime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			offset_y += 2 * deltatime / scale;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			view_angle -= deltatime;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			view_angle += deltatime;
			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			scale *= 1.02f;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			scale *= 0.98f;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		
		if (wheelpos < 0) {
			wheelpos = 1 - (0.03f * (speed+2));
		}
		else if (wheelpos > 0) {
			wheelpos = (0.04f * 1 / speed) + 1;
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

		if(speed < 2)
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


			GLfloat distance;
			GLfloat distance_x;
			GLfloat distance_y;

			GLfloat force;
			GLfloat force_x, force_y;
			force_x = 0.0f;
			force_y = 0.0f;



			for (unsigned int j = 0; j < objects.size(); j++) {
				if (i != j) {



					distance_x = (objects[j].position.x - objects[i].position.x);
					distance_y = (objects[j].position.y - objects[i].position.y);


					distance = sqrt(distance_x * distance_x + distance_y * distance_y);

					force = Gravity * (objects[i].mass * objects[j].mass) / (distance * distance);


					force_x += force * distance_x / distance;

					force_y += force * distance_y / distance;

					if (abs(distance_x) < ((float)WIDTH / HEIGHT * objects[i].mass + 1 + objects[j].mass + 1) / WIDTH && abs(distance_y) <= (objects[i].mass + 1 + objects[j].mass + 1) / HEIGHT) {

						int k = i;
						int l = j;
						if (objects[i].mass <= objects[j].mass) {
							std::swap(k, l);

							force_x = 0.0f;
							force_y = 0.0f;

							i++;
							j = 0;
						}



						objects[k].massgain += objects[l].mass;

						//objects[k].position.x += distance_x;
						//objects[k].position.y += distance_y;

						//objects[k].velocity.x += objects[l].velocity.x * (objects[j].mass / objects[i].mass);
						//objects[k].velocity.y += objects[l].velocity.y * (objects[j].mass / objects[i].mass);


						objects.erase(objects.begin() + l);

						if (l > k)
							i--;

						//std::swap(objects[l], objects[objects.size() - 1]);


						if (i >= objects.size() - 1)
							i = 0;

						//objects.pop_back();

					}


				}
			}

			objects[i].mass += objects[i].massgain * deltatime * speed * 7;
			objects[i].massgain *= 1 - deltatime * speed * 4;

			if (objects[i].massgain < 0.1f)
				objects[i].massgain = 0;

			acceleration_x = force_x / objects[i].mass;
			acceleration_y = force_y / objects[i].mass;

			objects[i].velocity.x += acceleration_x * 2 * deltatime * speed;
			//objects[i].velocity.x *= 0.999f;
			objects[i].velocity.y += acceleration_y * 2 * deltatime * speed;
			//objects[i].velocity.y *= 0.999f;
			/*
			if (objects[i].position.x + (objects[i].mass + 1) / (WIDTH) > 1.0f * (float)WIDTH/HEIGHT - objects[0].position.x) {

				objects[i].position.x = -(objects[i].mass + 1) / (WIDTH) + 1.0f * (float)WIDTH / HEIGHT - objects[0].position.x;
				objects[i].velocity.x = -objects[i].velocity.x * 0.95f;
			}

			if (objects[i].position.x - (objects[i].mass + 1) / (WIDTH) < -1.0f * (float)WIDTH / HEIGHT - objects[0].position.x) {

				objects[i].position.x = (objects[i].mass + 1) / (WIDTH) - 1.0f * (float)WIDTH / HEIGHT - objects[0].position.x;
				objects[i].velocity.x = -objects[i].velocity.x * 0.95f;
			}

			if (objects[i].position.y + (objects[i].mass + 1) / (HEIGHT) > 1.0f - objects[0].position.y) {

				objects[i].position.y = -(objects[i].mass + 1) / (HEIGHT) + 1.0f - objects[0].position.y;
				objects[i].velocity.y = -objects[i].velocity.y * 0.95f;
			}

			if (objects[i].position.y - (objects[i].mass + 1) / (HEIGHT) < -1.0f - objects[0].position.y) {

				objects[i].position.y = (objects[i].mass + 1) / (HEIGHT) - 1.0f - objects[0].position.y;
				objects[i].velocity.y = -objects[i].velocity.y * 0.95f;
			}
			*/
			objects[i].position.x += objects[i].velocity.x * deltatime * speed;
			objects[i].position.y += objects[i].velocity.y * deltatime * speed;

			//ball[0] = (objects[i].position.x - objects[0].position.x)* ((float)HEIGHT/WIDTH);
			//ball[1] = objects[i].position.y - objects[0].position.y;
			/*
			GLfloat r = sqrt(pow(objects[i].position.x, 2) + pow(objects[i].position.y, 2));
			GLfloat angle = (objects[i].position.y / objects[i].position.x);
			GLfloat x = cos(view_angle + angle) * r;
			GLfloat y = sin(view_angle + angle) * r;

			GLfloat a_offset_x = cos(view_angle) * sqrt(pow(offset_x, 2) + pow(offset_y, 2));
			GLfloat a_offset_y = sin(view_angle) * sqrt(pow(offset_x, 2) + pow(offset_y, 2));


			ball[0] = scale * (x + a_offset_x) * ((float)HEIGHT / WIDTH);
			ball[1] = scale * (y + a_offset_y);
			*/

			ball[0] = scale * (objects[i].position.x + offset_x) * ((float)HEIGHT / WIDTH);
			ball[1] = scale * (objects[i].position.y + offset_y);

			glPointSize(scale * objects[i].mass + 2.0f);

			glBindVertexArray(VAO_ball);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_ball);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);

			glUniform4fv(Color, 1, &glm::vec4(
				5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
				-0.8f + 5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
				1.0f - 5 * sqrt(pow(objects[i].velocity.x, 2) + pow(objects[i].velocity.y, 2)),
				1.0f
				)[0]);

			glDrawArrays(GL_POINTS, 0, 1);

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
		deltatime = glfwGetTime() - stdtime;


		//if(!(counter%5))
			//std::cout << "FPS:\t" << 1 / deltatime << std::endl;

		counter++;

	}


	std::cout << "Press any key to continue. . ." << std::endl;

	int tmp;

	std::cin >> tmp;

	return 0;

}