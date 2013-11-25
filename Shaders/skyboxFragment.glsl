#version 150 core

uniform samplerCube cubeTex ;
uniform vec3 cameraPos;
uniform vec3 colourMod;
uniform float sunHeight;

in Vertex {
	vec3 normal ;
} IN ;

out vec4 gl_FragColor ;

vec3 lerp(float weight, vec3 lhs, vec3 rhs){
	return (1-weight)*lhs + weight * rhs;
}

void main ( void ) {

	vec3 ambient;
	float weightVal = clamp(sunHeight,100,500)/400;
	ambient = lerp(weightVal, colourMod, vec3(0.9,0.9,0.9));

	vec4 newcolourMod = vec4(ambient.r, ambient.g, ambient.b, 1.0);
	gl_FragColor = newcolourMod * texture ( cubeTex , normalize ( IN . normal ));
}
