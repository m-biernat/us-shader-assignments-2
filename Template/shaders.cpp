#include <GL/glew.h>
#include <iostream>
#include <fstream>

#include "shaders.h"

/*------------------------------------------------------------------------------------------
** funkcja wczytujaca z pliku kod zrodlowy shadera
** filename - nazwa pliku z kodem zrodlowym shadera
** funkcja zwraca tablice z kodem zrodlowym shadera
**------------------------------------------------------------------------------------------*/
GLchar* loadShaderSource(std::string filename)
{
	std::ifstream file;
	file.open(filename, std::ios::in);

	if (!file)
	{
		std::cerr << "Nie mozna otworzyc pliku: " << filename.c_str() << std::endl;
		return nullptr;
	}

	file.seekg(0, std::ios::end);
	unsigned long long length = file.tellg();

	file.seekg(std::ios::beg);

	GLchar* buffer = new GLchar[length + 1];

	unsigned int i = 0;
	while (file.good())
	{
		buffer[i] = file.get();
		if (!file.eof())
			++i;
	}

	buffer[i] = NULL;
	file.close();

	return buffer;
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca okreslony rodzaj shadera
** filename - nazwa pliku z kodem zrodlowym shadera
** shaderType - typ tworzonego shadera
** shader - referencja na identyfikator tworzonego w funkcji shadera
** funkcja zwraca true jesli powiedzie sie tworzenie shadera
**------------------------------------------------------------------------------------------*/
bool createShader(std::string filename, GLenum shaderType, GLuint& shader)
{
	shader = glCreateShader(shaderType); // utworzenie identyfikatora shadera

	const GLchar* source = loadShaderSource(filename); // wczytanie kodu zrodlowego shadera wierzchokow
	if (source != nullptr)
	{
		glShaderSource(shader, 1, &source, nullptr); // ustawienie kodu zrodlowego shadera
		delete[] source;

		glCompileShader(shader); // kompilacja shadera

		GLint compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == 0)
		{
			std::cerr << "Blad przy kompilacji: " << filename.c_str() << std::endl;
			printShaderInfoLog(shader); // wyswietlenie logu shadera

			return false;
		}
	}
	else
		return false;

	return true;
}

/*------------------------------------------------------------------------------------------
** funkcja wyswietla zawartosc logu shadera
** shader - identyfikator shadera
**------------------------------------------------------------------------------------------*/
void printShaderInfoLog(GLuint shader)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = new char[infologLength];
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}
}

/*------------------------------------------------------------------------------------------
** funkcja wyswietla zawartosc logu programu cieniowania
** program - identyfikator programu cieniowania
**------------------------------------------------------------------------------------------*/
void printProgramInfoLog(GLuint program)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = new char[infologLength];
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
** vertexShaderFilename - nazwa pliku z kodem zrodlowym shadera wierzcholkow
** fragmentShaderFilename - nazwa pliku z kodem zrodlowym shadera fragmentow
** shaderProgram - referencja na identyfikator tworzonego w funkcji programu
** funkcja zwraca true jesli powiedzie sie tworzenie programu cieniowania
**------------------------------------------------------------------------------------------*/
bool setupShaders(std::string vertexShaderFilename, std::string fragmentShaderFilename, GLuint& shaderProgram)
{
	shaderProgram = glCreateProgram(); // utworzenie identyfikatora programu cieniowania

	GLuint vertexShader;
	if (!createShader(vertexShaderFilename, GL_VERTEX_SHADER, vertexShader))
	{
		glDeleteShader(vertexShader);
		glDeleteProgram(shaderProgram);

		return false;
	}

	GLuint fragmentShader;
	if (!createShader(fragmentShaderFilename, GL_FRAGMENT_SHADER, fragmentShader))
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);

		return false;
	}

	glAttachShader(shaderProgram, vertexShader); // dolaczenie shadera wierzcholkow
	glAttachShader(shaderProgram, fragmentShader); // dolaczenie shadera fragmentow

	glLinkProgram(shaderProgram); // linkowanie programu cieniowania

	GLint linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0)
	{
		std::cerr << "Blad przy linkowaniu programu cieniowania (" << vertexShaderFilename.c_str() << ", " << fragmentShaderFilename.c_str() << ")\n";
		printProgramInfoLog(shaderProgram); // wyswietlenie logu linkowania

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);

		return false;
	}

	return true;
}