#version 410

uniform mat4 matrix;
uniform mat4 perspective;
uniform bool noColor;

in vec4 vertex;
in vec4 color;

out vec4 vertColor;

void main( void )
{
    if (noColor) vertColor = vec4(0.4, 0.2, 0.6, 1.0);
    else vertColor = color;
    gl_Position = perspective * matrix * vertex;
}
