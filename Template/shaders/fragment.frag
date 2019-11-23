#version 330

in vec4 color; // kolor interpolowany z shadera wierzcholkow

out vec4 fColor; // wyjsciowy kolor fragmentu

void main()
{
	fColor = color;
}