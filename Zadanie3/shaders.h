#ifndef __SHADERS_H__
#define __SHADERS_H__

GLchar* loadShaderSource(std::string filename);
bool createShader(std::string filename, GLenum shaderType, GLuint& shader);
void printShaderInfoLog(GLuint shader);
void printProgramInfoLog(GLuint program);
bool setupShaders(std::string vertexShaderFilename, std::string fragmentShaderFilename, GLuint& shaderProgram);

#endif /* __SHADERS_H__ */