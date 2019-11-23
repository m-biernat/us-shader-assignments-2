#version 330

uniform mat4 modelViewMatrix; // macierz model-widok
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
 
void main()
{
	gl_Position = modelViewMatrix * vPosition;
}