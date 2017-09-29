#version 330 core

in vec3 vertex;
in vec3 color;
in vec3 normal;

// Values that stay constant for the whole mesh.
uniform mat4 matrix;
uniform mat4 perspective;


void main(){
	vec3 vertColor = color;
	vec3 vertNormal = normal;
	gl_Position = perspective * matrix * vec4(vertex, 1.0);
}

