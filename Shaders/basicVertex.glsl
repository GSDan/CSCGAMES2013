#version 150 core


in  vec3 position;
in  vec3 colour;

out Vertex {
	vec4 colour;
} OUT;

void main(void)	{
	gl_Position	  = vec4(position, 1.0);
	OUT.colour    = vec4(colour,1);
}