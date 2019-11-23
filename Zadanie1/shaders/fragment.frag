#version 330
 
uniform vec4 color; // kolor obiektu

out vec4 fColor;
 
void main()
{
    fColor = color;
}