#include"Renderer.h"

Renderer :: Renderer ( Window & parent ) : OGLRenderer ( parent ) {

	
	resolution = Vector3((float) width, (float) height, 1);
	camera = new Camera();
	heightMap = new HeightMap("../../Textures/world.raw");
	quad = Mesh::GenerateQuad();

	camera->SetPosition ( Vector3 ( RAW_WIDTH * HEIGHTMAP_X / 2.0f,500.0f, RAW_WIDTH * HEIGHTMAP_X ));
	light = new Light ( Vector3 (( RAW_HEIGHT * HEIGHTMAP_X / 2.0f ) ,450.0f,( RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),Vector4 (0.9f ,0.9f ,1.0f ,1) ,( RAW_WIDTH * HEIGHTMAP_X ) / 1.1f);
	reflectShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader ("../../Shaders/skyboxVertex.glsl","../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/bumpFragment.glsl");
	underwaterShader = new Shader("../../Shaders/bumpVertex.glsl","../../Shaders/underwaterFrag.glsl");

	if (! reflectShader -> LinkProgram () || ! lightShader -> LinkProgram () ||! skyboxShader -> LinkProgram ()) {
		return ;
	}

	quad->SetTexture(SOIL_load_OGL_texture("../../Textures/anotherwater.JPG",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	
	heightMap -> SetTexture ( SOIL_load_OGL_texture ("../../Textures/Barren Reds.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMap ( SOIL_load_OGL_texture ("../../Textures/Barren RedsDOT3.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));

	cubeMap = SOIL_load_OGL_cubemap ("../../Textures/rusted_west.jpg","../../Textures/rusted_east.jpg","../../Textures/rusted_up.jpg",
									"../../Textures/rusted_down.jpg","../../Textures/rusted_south.jpg","../../Textures/rusted_north.jpg",SOIL_LOAD_RGB ,SOIL_CREATE_NEW_ID , 0);
	if (!cubeMap || !quad->GetTexture()) {
		return ;
	}
	SetTextureRepeating (quad->GetTexture (), true );
	SetTextureRepeating (heightMap->GetTexture (), true );
	SetTextureRepeating (heightMap->GetBumpMap (), true );

	init = true ;
	waterRotate = 0.0f;
	projMatrix = Matrix4 :: Perspective (1.0f ,15000.0f,
		(float ) width / (float )height , 45.0f);
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
	glEnable ( GL_TEXTURE_CUBE_MAP_SEAMLESS );
}

Renderer ::~ Renderer ( void ) {
	delete camera ;
	delete heightMap ;
	delete quad ;
	delete reflectShader ;
	delete skyboxShader ;
	delete lightShader ;
	delete light ;
	delete underwaterShader;
	currentShader = 0;
}

void Renderer :: UpdateScene (float msec ) {
	camera -> UpdateCamera ( msec/2.0f );
	viewMatrix = camera -> BuildViewMatrix ();
	waterRotate += msec / 2000.0f ;
}

void Renderer :: RenderScene () {
	glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	DrawSkybox ();
	DrawHeightmap ();
	DrawWater ();
	//DrawUnder

	SwapBuffers ();
}

void Renderer :: DrawSkybox () {
	glDepthMask ( GL_FALSE );
	SetCurrentShader ( skyboxShader );

	UpdateShaderMatrices ();
	quad -> Draw ();

	glUseProgram (0);
	glDepthMask ( GL_TRUE );
}

void Renderer :: DrawHeightmap () {
	SetCurrentShader ( lightShader );
	SetShaderLight (* light );	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ());

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTex") , 0);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"bumpTex") , 1);

	modelMatrix.ToIdentity ();
	textureMatrix.ToIdentity ();

	UpdateShaderMatrices ();

	heightMap -> Draw ();

	glUseProgram (0);
}

void Renderer :: DrawUnderwater(Window & parent){
	SetCurrentShader(underwaterShader);
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(),"iResolution"),1, (float*)&resolution );
	glUniform1f ( glGetUniformLocation ( currentShader -> GetProgram () ,"iGlobalTime") , parent.elapsedMS);
	glUseProgram(0);
}

void Renderer :: DrawWater () {
	SetCurrentShader ( reflectShader );
	SetShaderLight (* light );
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ());

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTex") , 0);

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"cubeTex") , 2);

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP , cubeMap );

	float heightX = ( RAW_WIDTH * HEIGHTMAP_X );

	float heightY = 256 * HEIGHTMAP_Y/ 4.0f ;

	float heightZ = ( RAW_HEIGHT * HEIGHTMAP_Z );

	modelMatrix =
	Matrix4 :: Translation ( Vector3 ( heightX , heightY , heightZ )) *
	Matrix4 :: Scale ( Vector3 ( heightX ,1 , heightZ )) *
	Matrix4 :: Rotation (90 , Vector3 (1.0f ,0.0f ,0.0f ));

	textureMatrix = Matrix4 :: Scale ( Vector3 (10.0f ,10.0f ,10.0f )) *
	Matrix4 :: Rotation ( waterRotate , Vector3 (0.0f ,0.0f ,1.0f ));
	UpdateShaderMatrices ();

	quad -> Draw ();

	glUseProgram (0);
}