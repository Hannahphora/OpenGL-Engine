#include "Shader.h"

ShaderProgram::ShaderProgram(const char* vertPath, const char* fragPath) {

	// retrieve source code from files

	std::string vertSrc, fragSrc;
	std::ifstream vertFile, fragFile;

	vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// open files
		vertFile.open(vertPath);
		fragFile.open(fragPath);
		std::stringstream vertStream, fragStream;
		// read file contents into streams
		vertStream << vertFile.rdbuf();
		fragStream << fragFile.rdbuf();
		// close file handlers
		vertFile.close();
		fragFile.close();
		// convert stream to string
		vertSrc = vertStream.str();
		fragSrc = fragStream.str();
	}
	catch (std::ifstream::failure e) {
		fprintf(stderr, "Error reading shader file\n");
	}

	const char* vertCode = vertSrc.c_str();
	const char* fragCode = fragSrc.c_str();

	// compile and link shaders

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

// uniform utility funcs

void ShaderProgram::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}