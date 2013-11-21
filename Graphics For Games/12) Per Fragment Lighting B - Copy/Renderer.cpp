#include"Renderer.h"


Renderer :: Renderer ( Window & parent ) : OGLRenderer ( parent ) {
	
	//create the island's node assets
	Island :: CreateSphere();
	Island:: CreateHell();

	//how long the day should go on for
	dayLengthSeconds = 60;
	isNight = false;

	initTimer = 2.0f;

	//ambient colours for night and day
	minAmbient = Vector3(0.0f, 0.0f, 0.03f);
	maxAmbient = Vector3(0.05f, 0.05f, 0.05f);

	//the colour the skybox fragments are multiplied by
	skyColourMod = Vector3(1,1,1);

	camera = new Camera();
	heightMap = new HeightMap("../../Textures/world.raw");
	quad = Mesh::GenerateQuad();	

	camera->SetPosition ( Vector3 ( RAW_WIDTH * HEIGHTMAP_X / 2.0f,500.0f, RAW_WIDTH * HEIGHTMAP_X ));

	//assign shaders
	sunlight = new Light ( Vector3 (0,0,0),Vector4 (1.0f ,1.0f ,1.0f ,1) , 6000);
	ghostlight = new Light ( Vector3 (1600,470,1800),Vector4 (1,1,1,1) , 0);
	reflectShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader ("../../Shaders/skyboxVertex.glsl","../../Shaders/skyboxFragment.glsl");
	lightShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/bumpFragment.glsl");
	sunShader  = new Shader("../../Shaders/SceneVertex.glsl","../../Shaders/SceneFragment.glsl");		

	//link shaders
	if (! reflectShader -> LinkProgram () || ! lightShader -> LinkProgram () ||! skyboxShader -> LinkProgram () || !sunShader->LinkProgram() ){
		return ;
	}

	root = new Island();

	quad->SetTexture(SOIL_load_OGL_texture("../../Textures/anotherwater.JPG",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	
	//set textures and bump maps for the two materials
	heightMap -> SetTextureLower ( SOIL_load_OGL_texture ("../../Textures/sand.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapLower ( SOIL_load_OGL_texture ("../../Textures/sandbump.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetTextureUpper ( SOIL_load_OGL_texture ("../../Textures/rock.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapUpper ( SOIL_load_OGL_texture ("../../Textures/Barren RedsDOT3.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));

	//set cube map
	dayCubeMap = SOIL_load_OGL_cubemap ("../../Textures/ss_ft.tga","../../Textures/ss_bk.tga","../../Textures/ss_up.tga",
									"../../Textures/ss_dn.tga","../../Textures/ss_rt.tga","../../Textures/ss_lf.tga",SOIL_LOAD_RGB ,SOIL_CREATE_NEW_ID , 0);
	

	if (!dayCubeMap || !quad->GetTexture()) {
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
	delete sunShader;
	delete root;
	Island::DeleteSphere();
	Island::DeleteHell();
	delete sunlight ;
	delete ghostlight;
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
		ghostlight->SetRadius(400);
		skyColourMod = Vector3(0.3f, 0.3f, 0.4f);
	}else if(sunlight->GetRadius() == 0){
		skyColourMod = Vector3(1,1,1);
		isNight = false;
		sunlight->SetRadius(8000);
		ghostlight->SetRadius(0);

	}

}

void Renderer :: RenderScene () {
	glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	DrawSkybox ();
	DrawHeightmap ();
	DrawWater ();

	//no need to draw the sun sphere at night
	if(!isNight){
		SetCurrentShader(sunShader);
		glUseProgram(sunShader -> GetProgram());
		UpdateShaderMatrices();
		glUniform1i ( glGetUniformLocation ( sunShader -> GetProgram (),"diffuseTex"), 1);
		DrawNode(root->ball);
	}else{
		//draw hell knight at night!
		SetCurrentShader(sunShader);
		glUseProgram(sunShader -> GetProgram());
		UpdateShaderMatrices();
		root->hellNode->SetColour(Vector4(1,1,1,0.4f));
		glUniform1i ( glGetUniformLocation ( sunShader -> GetProgram (),"diffuseTex"), 1);
		DrawNode(root->hellNode);
	}

	glUseProgram(0);

	SwapBuffers ();
}

void Renderer :: DrawSkybox () {
	glDepthMask ( GL_FALSE );
	SetCurrentShader ( skyboxShader );
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"colourMod"), 1, (float*)&skyColourMod); 
	UpdateShaderMatrices ();
	quad -> Draw ();

	glUseProgram (0);
	glDepthMask ( GL_TRUE );
}

void Renderer :: DrawHeightmap () {
	
	SetCurrentShader ( lightShader );
	if(isNight)
		SetShaderLight (* ghostlight );	else		SetShaderLight (* sunlight );		glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ());
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
	if(isNight)
		SetShaderLight (* ghostlight );	else		SetShaderLight (* sunlight );
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ()); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMax"), 1, (float*)&maxAmbient); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMin"), 1, (float*)&minAmbient); 
	glUniform1i(glGetUniformLocation ( currentShader -> GetProgram () ,"isNight"), isNight);

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTex") , 0);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"cubeTex") , 2);

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP , dayCubeMap );

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
		glUniformMatrix4fv (glGetUniformLocation ( sunShader -> GetProgram (),"modelMatrix"), 1,false ,(float *)& transform );
		glUniform4fv ( glGetUniformLocation ( sunShader -> GetProgram (),"nodeColour"),1,( float *)&n-> GetColour ());
		glUniform1i ( glGetUniformLocation ( sunShader -> GetProgram (),"useTexture"),( int )n-> GetMesh()->GetTexture());
		n->Draw(*this);
	}
	for (vector < SceneNode * >:: const_iterator
		i = n-> GetChildIteratorStart ();
		i != n-> GetChildIteratorEnd (); ++i) {
			DrawNode (*i);
	}
}