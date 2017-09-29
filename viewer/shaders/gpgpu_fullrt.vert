#version 410

// Standard parameters of a VAO
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 texcoords;

out vec2 textCoords;

void main( void )
{
	// You need to use color/normal/textcoords. Bad things can happen otherwise
	vec4 vertColor = vec4(color, 1.0);
	vec3 vertNormal = normal;
	/*
	 * Compute texture coordinate by simply
	 * interval-mapping from [-1..+1] to [0..1]
	 */
	textCoords = vertex.xy * 0.5 + vec2(0.5, 0.5);
	gl_Position = vec4(vertex, 1.0);
}
