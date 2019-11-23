#version 330

in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec4 vColor; // kolor wierzcholka

out vec4 color; // kolor przekazywany do shadera fragmentow

void main()
{
	color = vColor;

	gl_Position = vPosition;
}