#version 150 core

uniform sampler2D diffuseTex;

uniform vec3 cameraPos ;
uniform vec4 lightColour[2] ;
uniform vec3 lightPos[2] ;
uniform float lightRadius[2] ;


in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main(void) {


vec4 totalLights; 
for(int i = 0 ; i<1 ; i++)
{
	vec4 diffuse = texture ( diffuseTex , IN.texCoord );
	vec3 incident = normalize ( lightPos[i] - IN.worldPos );
	
	float lambert   = max (0.0 , dot ( incident , IN.normal ));
	float dist = length ( lightPos[i] - IN.worldPos );
	float atten = 1.0 - clamp ( dist / lightRadius[i] , 0.0 , 1.0);

	vec3 viewDir = normalize ( cameraPos - IN.worldPos);
	vec3 halfDir = normalize ( incident + viewDir);

	
	float rFactor = max (0.0,dot(halfDir,IN.normal));
	float sFactor = pow ( rFactor , 50.0 );
	

	vec3 colour = ( diffuse.rgb * lightColour[i].rgb );
	colour += ( lightColour[i].rgb * sFactor ) * 0.15;
	totalLights += vec4 ( colour * atten * lambert , diffuse.a);
	totalLights.rgb += ( diffuse.rgb * lightColour[i].rgb ) * 0.1;
}
	
	
//	gl_FragColor = vec4 ( colour * atten * lambert , diffuse.a);
	gl_FragColor = totalLights;

}