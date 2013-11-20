#include"Renderer.h"

Renderer :: Renderer ( Window & parent ) : OGLRenderer ( parent ) {
	
	Island :: CreateSphere();

	dayLengthSeconds = 60;
	isNight = false;

	initTimer = 2.0f;

	minAmbient = Vector3(0.0f, 0.0f, 0.03f);
	maxAmbient = Vector3(0.05f, 0.05f, 0.05f);

	camera = new Camera();
	heightMap = new HeightMap("../../Textures/world.raw");
	quad = Mesh::GenerateQuad();	

	camera->SetPosition ( Vector3 ( RAW_WIDTH * HEIGHTMAP_X / 2.0f,500.0f, RAW_WIDTH * HEIGHTMAP_X ));
	sunlight = new Light ( Vector3 (0,0,0),Vector4 (1.0f ,1.0f ,1.0f ,1) , 8000);
	reflectShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader ("../../Shaders/skyboxVertex.glsl","../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/bumpFragment.glsl");
	sceneShader  = new Shader("../../Shaders/SceneVertex.glsl","../../Shaders/SceneFragment.glsl");	

	if (! reflectShader -> LinkProgram () || ! lightShader -> LinkProgram () ||! skyboxShader -> LinkProgram () || !sceneShader->LinkProgram()) {
		return ;
	}

	root = new Island();

	quad->SetTexture(SOIL_load_OGL_texture("../../Textures/anotherwater.JPG",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	
	//set textures and bump maps for the two materials
	heightMap -> SetTextureLower ( SOIL_load_OGL_texture ("../../Textures/sand.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapLower ( SOIL_load_OGL_texture ("../../Textures/sandbump.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetTextureUpper ( SOIL_load_OGL_texture ("../../Textures/rock.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapUpper ( SOIL_load_OGL_texture ("../../Textures/Barren RedsDOT3.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));

	cubeMap = SOIL_load_OGL_cubemap ("../../Textures/rusted_west.jpg","../../Textures/rusted_east.jpg","../../Textures/rusted_up.jpg",
									"../../Textures/rusted_down.jpg","../../Textures/rusted_south.jpg","../../Textures/rusted_north.jpg",SOIL_LOAD_RGB ,SOIL_CREATE_NEW_ID , 0);

	if (!cubeMap || !quad->GetTexture()) {
		return ;
	}

	//set textures to repeat
	SetTextureRepeating (quad->GetTexture (), true );
	SetTextureRepeating (heightMap->GetTextureLower (), true );
	SetTextureRepeating (heightMap->GetBumpMapLower (), true );
	SetTextureRepeating (heightMap->GetTextureUpper (), true );
	SetTextureRepeating (heightMap->GetBumpMapUpper (), true );	

	init = true ;

	waterRotate = 0.2f;

	projMatrix = Matrix4 :: Perspective (1.0f ,15000.0f,(float ) width / (float )height , 55.0f);
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
	delete sceneShader;
	delete root;
	Island::DeleteSphere();
	delete sunlight ;
	currentShader = 0;
}

void Renderer :: UpdateScene (float msec ) {
	camera -> UpdateCamera ( msec/2.0f );
	viewMatrix = camera -> BuildViewMatrix ();
	waterRotate += msec / 2000.0f ;

	initTimer  += msec * 0.001;
	float y = cos(initTimer* 1.288 / (dayLengthSeconds/5)) * 1800;
    float z = 1542 + sin (initTimer* 1.288 / (dayLengthSeconds/5)) * 1800;
	sunlight->SetPosition(Vector3(1542,y,z));
	root->Update(Vector3(1542,y,z), msec);

	//use the sun's position to determine if currently day or night
	//if night, reduce the radius of the sun's light to 0
	if(sunlight->GetPosition().y < 130){
		isNight = true;
		sunlight->SetRadius(0);
	}else if(sunlight->GetRadius() == 0){
		isNight = false;
		sunlight->SetRadius(8000);
	}

}

void Renderer :: RenderScene () {
	glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	DrawSkybox ();
	DrawHeightmap ();
	DrawWater ();

	//no need to draw the sun sphere at night
	if(!isNight){
		SetCurrentShader(sceneShader);
		glUseProgram(sceneShader -> GetProgram());
		UpdateShaderMatrices();
		glUniform1i ( glGetUniformLocation ( sceneShader -> GetProgram (),"diffuseTex"), 1);
		DrawNode(root);
	}

	glUseProgram(0);

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
	SetShaderLight (* sunlight );		glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ());
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMax"), 1, (float*)&maxAmbient); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMin"), 1, (float*)&minAmbient); 
	glUniform1i(glGetUniformLocation ( currentShader -> GetProgram () ,"isNight"), isNight);

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTexLower") , 0);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"bumpTexLower") , 1);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTexUpper") , 2);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"bumpTexUpper") , 3);

	modelMatrix.ToIdentity ();
	textureMatrix.ToIdentity ();

	UpdateShaderMatrices ();

	heightMap -> Draw ();

	glUseProgram (0);
}

void Renderer :: DrawWater () {
	
	SetCurrentShader ( reflectShader );
	SetShaderLight (* sunlight );
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ()); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMax"), 1, (float*)&maxAmbient); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMin"), 1, (float*)&minAmbient); 
	glUniform1i(glGetUniformLocation ( currentShader -> GetProgram () ,"isNight"), isNight);

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTex") , 0);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"cubeTex") , 2);

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP , cubeMap );

	float heightX = ( RAW_WIDTH * HEIGHTMAP_X /2);

	float heightY = 256 * HEIGHTMAP_Y/ 4.0f ;

	float heightZ = ( RAW_HEIGHT * HEIGHTMAP_Z /2);

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

void Renderer :: DrawNode (SceneNode *n) {
	if(n->GetMesh ()) 
	{
		Matrix4 transform = n->GetWorldTransform ()* Matrix4 ::Scale (n->GetModelScale());
		glUniformMatrix4fv (glGetUniformLocation ( sceneShader -> GetProgram (),"modelMatrix"), 1,false ,(float *)& transform );
		glUniform4fv ( glGetUniformLocation ( sceneShader -> GetProgram (),"nodeColour"),1,( float *)&n-> GetColour ());
		glUniform1i ( glGetUniformLocation ( sceneShader -> GetProgram (),"useTexture"),( int )n-> GetMesh()->GetTexture());
		n->Draw(*this);
	}
	for (vector < SceneNode * >:: const_iterator
		i = n-> GetChildIteratorStart ();
		i != n-> GetChildIteratorEnd (); ++i) {
			DrawNode (*i);
	}
}