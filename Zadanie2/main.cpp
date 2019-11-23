#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "shaders.h"


const float ROTATION_OFFSET = 0.0f;
//const float ROTATION_OFFSET = 45.0f;

const float SCALE = 0.2f, OFFSET = 0.8f, DIFF = 0.4f;

const float SQUARE_COLOR[] = { 0.0f, 1.0f, 0.0f, 1.0f };
const float TRIANGLE_COLOR[] = { 1.0f, 0.0f, 0.0f, 1.0f };


constexpr int WIDTH = 600; // szerokosc okna
constexpr int HEIGHT = 600; // wysokosc okna

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholkow
GLuint colorLoc; // lokalizacja zmiennej jednorodnej - kolor rysowania prymitywu

GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok

glm::mat4 mvMatrix; // macierz model-widok

GLuint vao[2]; // identyfikatory VAO
GLuint buffers[2]; // identyfikatory VBO
//******************************************************************************************

void errorCallback(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onShutdown();
void initGL();
void setupShaders();
void setupBuffers();
void renderScene();

void renderTriangles();
void renderSquares();

int main(int argc, char* argv[])
{
	atexit(onShutdown);

	GLFWwindow* window;

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Zadanie 2", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "Blad: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	if (!GLEW_VERSION_3_3)
	{
		std::cerr << "Brak obslugi OpenGL 3.3\n";
		exit(2);
	}

	glfwSwapInterval(1); // v-sync on

	initGL();

	while (!glfwWindowShouldClose(window))
	{
		renderScene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja zwrotna do obslugi bledow biblioteki GLFW
** error - kod bledu
** description - opis bledu
**------------------------------------------------------------------------------------------*/
void errorCallback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

/*------------------------------------------------------------------------------------------
** funkcja zwrotna do obslugi klawiatury
** window - okno, kt�re otrzymalo zdarzenie
** key - klawisz jaki zostal nacisniety lub zwolniony
** scancode - scancode klawisza specyficzny dla systemu
** action - zachowanie klawisza (GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT)
** mods - pole bitowe zawierajace informacje o nacisnietych modyfikatorach (GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER)
**------------------------------------------------------------------------------------------*/
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(2, vao);
	glDeleteProgram(shaderProgram);
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	setupBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram))
		exit(3);

	vertexLoc = glGetAttribLocation(shaderProgram, "vPosition");
	colorLoc = glGetUniformLocation(shaderProgram, "color");

	mvMatrixLoc = glGetUniformLocation(shaderProgram, "modelViewMatrix");
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca VAO oraz zawarte w nim VBO z danymi o modelu
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays(2, vao); // generowanie identyfikatora VAO
	glGenBuffers(2, buffers); // generowanie identyfikatorow VBO

	// wspolrzedne wierzcholkow trojkata
	const float TRAINGLE_VERTICES[] =
	{
		0.0f, 0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, 0.0f, 1.0f
	};

	glBindVertexArray(vao[0]); // dowiazanie pierwszego VAO    	

	// VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TRAINGLE_VERTICES), TRAINGLE_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

	// wspolrzedne wierzcholkow kwadratu
	const float SQUARE_VERTICES[] =
	{
		-0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f
	};

	glBindVertexArray(vao[1]); // dowiazanie pierwszego VAO    	

	// VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE_VERTICES), SQUARE_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

	glBindVertexArray(0);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	renderTriangles();

	renderSquares();

	glBindVertexArray(0);
}

void renderTriangles()
{
	float x = -OFFSET, y = OFFSET;

	glBindVertexArray(vao[0]);
	glUniform4fv(colorLoc, 1, TRIANGLE_COLOR);

	float rotation = 90.0f - ROTATION_OFFSET;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				mvMatrix = glm::mat4(1.0f);
				mvMatrix = glm::translate(mvMatrix, glm::vec3(x, y, 0.0f));

				mvMatrix = glm::rotate(mvMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, -0.15f, 0.0f));

				mvMatrix = glm::scale(mvMatrix, glm::vec3(SCALE, SCALE, 0.0f));

				glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);

				rotation -= 90.0f;
			}
			x += DIFF;
		}
		x = -OFFSET;
		y -= DIFF;
	}
}

void renderSquares()
{
	float x = -OFFSET, y = OFFSET;

	glBindVertexArray(vao[1]);
	glUniform4fv(colorLoc, 1, SQUARE_COLOR);

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			mvMatrix = glm::mat4(1.0f);
			mvMatrix = glm::translate(mvMatrix, glm::vec3(x, y, 0.0f));
			mvMatrix = glm::rotate(mvMatrix, glm::radians(-ROTATION_OFFSET), glm::vec3(0.0f, 0.0f, 1.0f));
			mvMatrix = glm::scale(mvMatrix, glm::vec3(SCALE, SCALE, 0.0f));

			glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			x += DIFF;
		}
		x = -OFFSET;
		y -= DIFF;
	}
}