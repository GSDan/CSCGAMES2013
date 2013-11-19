#version 150 core

uniform sampler2D diffuseTexLower;
uniform sampler2D bumpTexLower;
uniform sampler2D diffuseTexUpper;
uniform sampler2D bumpTexUpper;

uniform vec3 cameraPos ;
uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform vec3 ambientMax;
uniform vec3 ambientMin;
uniform float weight;
uniform float lightRadius ;

in Vertex {
vec3 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 tangent ; // New !
vec3 binormal ; // New !
vec3 worldPos ;
} IN ;

out vec4 gl_FragColor ;

vec3 lerp(float weight, vec3 lhs, vec3 rhs){

	return (1-weight)*lhs + weight * rhs;

}

void main ( void ) {

	vec3 normal;
	vec4 diffuse;

	if(IN.worldPos.x > 2000.0 && IN.worldPos.y > 300.0){
		diffuse = texture ( diffuseTexUpper , IN.texCoord );			

	}else if(IN.worldPos.x > 1800.0 && IN.worldPos.y > 270.0){

		//Jesse, we need to cook! Make a mix of the two textures
		//1.0 = fully lower (sand)
		//0.0 = fully upper (rock)

		float texMix = 1.0 - (((IN.worldPos.x - 1800.0 )/200.0));

		//max is 1.0, min 0.0
		if(texMix < 0.0){
			texMix = 0.0;
		}else if(texMix > 1.0){
			texMix = 1.0;
		}

		
		diffuse = mix(texture2D(diffuseTexUpper,IN.texCoord), texture2D(diffuseTexLower,IN.texCoord), texMix); 
	}
	else{
		diffuse = texture ( diffuseTexLower , IN.texCoord );
	}


		float weightVal = abs(sin(weight));
		vec3 ambient = lerp(weightVal, ambientMin, ambientMax);

		mat3 TBN = mat3 ( IN . tangent , IN.binormal , IN.normal );
		normal = normalize ( TBN * ( texture ( bumpTexLower , IN.texCoord ). rgb * 2.0 - 1.0));

		vec3 incident = normalize ( lightPos - IN . worldPos );
		float lambert = max (0.0 , dot ( incident , normal )); 

		float dist = length ( lightPos - IN . worldPos );
		float atten = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

		vec3 viewDir = normalize ( cameraPos - IN . worldPos );
		vec3 halfDir = normalize ( incident + viewDir );

		float rFactor = max (0.0 , dot ( halfDir , normal )); 
		float sFactor = pow ( rFactor , 33.0 );

		vec3 colour = ( diffuse . rgb * lightColour . rgb );
		colour +=  ( lightColour . rgb * sFactor ) * 0.33;
		gl_FragColor = vec4 ( colour * atten * lambert , diffuse . a );
		gl_FragColor . rgb += ( diffuse . rgb * lightColour . rgb  * ambient.rgb) * 0.1;
		
	

	
}


