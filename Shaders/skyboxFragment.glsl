#version 150 core

uniform samplerCube cubeTex ;
uniform vec3 cameraPos ;
uniform vec3 colourMod;

in Vertex {
	vec3 normal ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
	vec4 newcolourMod = vec4(colourMod.r, colourMod.g, colourMod.b, 1.0);
	gl_FragColor = newcolourMod * texture ( cubeTex , normalize ( IN . normal ));
}
