#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm\glm.hpp>

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <random>

#include "Shader.h"

GLFWwindow* window;

const GLuint WIDTH = 1280;
const GLuint HEIGHT = 720;
int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);


	const GLuint numVerts = 150000;

	GLfloat vertices[numVerts];

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	Shader shader;
	shader.load("vertex.glsl", "fragment.glsl");
	shader.use();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glPointSize(2.0f);

	glEnable(GL_POINT_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(-2, 2);

	int counter = 0;

	while (!glfwWindowShouldClose(window)) {

		counter++;

		glfwPollEvents();

		GLdouble iteration = 10;

		GLdouble ImZ = 0.0f;

		GLdouble ReZ = 1.0f;

		GLdouble Z = 0.0f;

		GLboolean mandelbrot;

		GLdouble magnitude;

		GLdouble angle;
		
		int i = 0;

		GLdouble mTime = 1.2f;

		mTime = pow(mTime, glfwGetTime() + 0.5) - 0.85f;

		while (i < numVerts / 3){

			//GLdouble mTime = 1.4f;
			
			//mTime = pow(mTime, glfwGetTime()+0.5) - 0.85f;

			GLdouble offset_x = 1.85f;

			GLdouble offset_y = 0.0f;

			GLdouble ReC = (distribution(generator))/mTime - offset_x;

			GLdouble ImC = (distribution(generator))/mTime - offset_y;

			ReZ = ReC;
			ImZ = ImC;

			iteration = glfwGetTime()*0.6f;

			mandelbrot = true;
				
				for (GLuint j = 0; j < iteration; j++) {
					magnitude = sqrt(ReZ * ReZ + ImZ * ImZ);

					angle = acos(ReZ / magnitude);

					if (magnitude >= 2 || magnitude <= -2) {
						mandelbrot = false;
						break;
					}
					ReZ = (ReZ * ReZ - ImZ * ImZ) + ReC;

					ImZ = (2 * ReZ * ImZ) + ImC;
				}

				//ReZ = 0.0f;
				//ImZ = 0.0f;

				if(mandelbrot){
					vertices[i * 3] =  0.5f * (ReC + offset_x) * mTime;
					vertices[i * 3 + 1] = 0.5f * (ImC + offset_y) * mTime;
					vertices[i * 3 + 2] = 0.0f;
					
				}
				else {
					vertices[i * 3] = vertices[(i - 1) * 3];
					vertices[i * 3 + 1] = vertices[(i - 1) * 3 + 1];
					vertices[i * 3 + 2] = 0.0f;
					
				}
				i++;


			}

		glClear(GL_COLOR_BUFFER_BIT);


		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, numVerts/3);

		glfwSwapBuffers(window);


	}

	std::cout << "Press any key to continue. . ." << std::endl;

	_getch();

	return 0;

}