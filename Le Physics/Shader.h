#pragma once

#include <GL\glew.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader
{
public:
	Shader();
	
	void load(const GLchar* vertexpath, const GLchar* fragmentpath);

	int get();

	void use();

private:

	GLuint Program;
};

