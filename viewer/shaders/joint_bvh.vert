#version 410

uniform mat4 matrix;
uniform mat4 perspective;

in vec4 vertex;
in vec4 color;

out vec4 vertColor;

void main( void )
{
    vertColor = color;
    gl_Position = perspective * matrix * vertex;
}
