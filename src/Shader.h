#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram {
public:

	uint32_t id;

	ShaderProgram(const char* vertPath, const char* fragPath);

	void use();

private:

	void checkErrors(uint32_t id, std::string type);

};