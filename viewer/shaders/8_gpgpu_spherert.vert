#version 410

// Standard parameters of a VAO
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 texcoords;

out vec4 vertColor;
out vec3 vertNormal;
out vec2 textCoords;
out vec3 position;

void main( void )
{
    // You need to use color/normal/textcoords. Bad things can happen otherwise
    vertColor = vec4(color, 1.0);
    vertNormal = normal;
    textCoords = texcoords;

    position = vertex;
    gl_Position = vec4(vertex, 1.0);
}
