#include "Shader.h"

ShaderProgram::ShaderProgram(const char* vertPath, const char* fragPath) {

	// 1) retrieve source code from files

	std::string vertSrc, fragSrc;
	std::ifstream vertFile, fragFile;

	vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vertFile.open(vertPath);
		fragFile.open(fragPath);
		std::stringstream vertStream, fragStream;
		
		vertStream << vertFile.rdbuf();
		fragStream << fragFile.rdbuf();

		vertFile.close();
		fragFile.close();

		vertSrc = vertStream.str();
		fragSrc = fragStream.str();
	}
	catch (std::ifstream::failure e) {
		fprintf(stderr, "Error reading shader file\n");
	}

	const char* vertCode = vertSrc.c_str();
	const char* fragCode = fragSrc.c_str();

	// 2) compile and link shaders

	// vert shader
	uint32_t vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertCode, nullptr);
	glCompileShader(vert);
	checkErrors(vert, "VERTEX");
	// frag shader
	uint32_t frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragCode, nullptr);
	glCompileShader(frag);
	checkErrors(frag, "FRAGMENT");
	// shader program
	id = glCreateProgram();
	glAttachShader(id, vert);
	glAttachShader(id, frag);
	glLinkProgram(id);
	checkErrors(id, "PROGRAM");

	// cleanup
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void ShaderProgram::use() {
	glUseProgram(id);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::checkErrors(uint32_t id, std::string type) {
	int success;
	char infoLog[512];
	if (type != "PROGRAM") {
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(id, 512, nullptr, infoLog);
			fprintf(stderr, "Error compiling %s shader\n%s\n", type.c_str(), infoLog);
		}
	}
	else {
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, nullptr, infoLog);
			fprintf(stderr, "Error linking %s program\n%s\n", type.c_str(), infoLog);
		}
	}
}