#version 330
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat4 projectionMatrix; // macierz projekcji
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
 
void main()
{
    gl_Position = projectionMatrix * modelViewMatrix * vPosition;
}