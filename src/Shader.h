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

	// uniform utility funcs
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:

	void checkErrors(uint32_t id, std::string type);

};