#version 150 core

uniform sampler2D diffuseTex ;
uniform samplerCube cubeTex ;

uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform vec3 cameraPos ;
uniform vec3 ambientMax;
uniform vec3 ambientMin;
uniform bool isNight;
uniform float lightRadius ;

in Vertex {
vec4 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 worldPos ;
} IN ;

out vec4 gl_FragColor ;

vec3 lerp(float weight, vec3 lhs, vec3 rhs){
	return (1-weight)*lhs + weight * rhs;
}

void main ( void ) {

	//float weightVal = abs(sin(weight));
	//vec3 ambient = lerp(weightVal, ambientMin, ambientMax);
		
	vec3 ambient;

	if(isNight){
		ambient = ambientMin;
	}else{
		ambient = ambientMax;
	}	

	vec4 diffuse = texture ( diffuseTex , IN . texCoord ) * IN . colour ;
	vec3 incident = normalize ( IN . worldPos - cameraPos );
	float dist = length ( lightPos - IN . worldPos );
	float atten = 1.0 - clamp ( dist / lightRadius , 0.2 , 0.7);
	vec4 reflection = texture ( cubeTex ,
	reflect ( incident , normalize ( IN . normal )));
	vec4 newAmbient = vec4(ambient.r, ambient.g, ambient.b, 1.0);
	gl_FragColor = newAmbient + ( lightColour * diffuse * atten )*( diffuse + reflection );
}