#include"Renderer.h"
#include <string> 

Renderer :: Renderer ( Window & parent ) : OGLRenderer ( parent ) {

	//create the island's node assets
	Island :: CreateSphere();
	Island:: CreateHell();

	//how long the day should go on for
	dayLengthSeconds = 120; //default is 2 mins
	isNight = false;
	initTimer = 2.0f;

	//start with no snow on ground
	isSnowing = false;
	snowAmount = 0.0f;

	//ambient colours for night and day
	minAmbient = Vector3(0.0f, 0.0f, 0.07f);
	maxAmbient = Vector3(0.05f, 0.05f, 0.05f);

	//the colour the skybox fragments are multiplied by
	skyColourMod = Vector3(0.2f,0.2f,0.25f);

	camera = new Camera();
	heightMap = new HeightMap("../../Textures/world.raw");
	quad = Mesh::GenerateQuad();	

	camera->SetPosition ( Vector3 ( RAW_WIDTH * HEIGHTMAP_X / 2.0f,500.0f, RAW_WIDTH * HEIGHTMAP_X ));

	//assign shaders
	sunlight = new Light ( Vector3 (0,0,0),Vector4 (1.0f ,1.0f ,1.0f ,1) , 6000);
	ghostlight = new Light ( Vector3 (1600,470,1800),Vector4 (1,0.7,0.7,1) , 0);
	reflectShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/reflectFragment.glsl");
	skyboxShader = new Shader ("../../Shaders/skyboxVertex.glsl","../../Shaders/skyboxFragment.glsl");
	textureShader = new Shader ("../../Shaders/TexturedVertex.glsl","../../Shaders/TexturedFragment.glsl");
	lightShader = new Shader ("../../Shaders/bumpVertex.glsl","../../Shaders/bumpFragment.glsl");
	sunShader  = new Shader("../../Shaders/SceneVertex.glsl","../../Shaders/SceneFragment.glsl");		
	particleShader = new Shader("../../Shaders/vertex.glsl","../../Shaders/fragment.glsl", "../../Shaders/geometry.glsl");
	textShader = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
	
	//link shaders
	if (! reflectShader -> LinkProgram () || ! lightShader -> LinkProgram () ||! skyboxShader -> LinkProgram () || !sunShader->LinkProgram()
		|| !particleShader->LinkProgram() || !textShader->LinkProgram() || !textureShader->LinkProgram()){
			return ;
	}	

	//create new particle emitters
	emitter = new ParticleEmitter();
	snowMachine = new ParticleEmitter();
	cloudMachine = new ParticleEmitter();
	cloudMachine->SetTexture(SOIL_load_OGL_texture ("../../Textures/Cartoon_Cloud1.png",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , 0));

	root = new Island();

	quad->SetTexture(SOIL_load_OGL_texture("../../Textures/anotherwater.JPG",SOIL_LOAD_AUTO , SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));

	//set textures and bump maps for the two materials
	heightMap -> SetTextureLower ( SOIL_load_OGL_texture ("../../Textures/sand.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapLower ( SOIL_load_OGL_texture ("../../Textures/sandbump.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetTextureUpper ( SOIL_load_OGL_texture ("../../Textures/rock.JPG", SOIL_LOAD_AUTO ,SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetBumpMapUpper ( SOIL_load_OGL_texture ("../../Textures/Barren RedsDOT3.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));
	heightMap -> SetTextureSnow (SOIL_load_OGL_texture ("../../Textures/anotherSnow.jpg", SOIL_LOAD_AUTO ,	SOIL_CREATE_NEW_ID , SOIL_FLAG_MIPMAPS ));

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
	SetTextureRepeating (heightMap->GetTextureSnow(), true );	

	init = true ;

	waterRotate = 0.2f;

	//setup basic font
	basicFont = new Font(SOIL_load_OGL_texture("../../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	projMatrix = Matrix4 :: Perspective (1.0f ,15000.0f,(float ) width / (float )height , 55.0f);
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_TEXTURE_CUBE_MAP_SEAMLESS );
}

inline float clamp(float x, float a, float b)
{
	return x < a ? a : (x > b ? b : x);
}

inline int lerp(float weight, int lhs, int rhs){
	return (1-weight)*lhs + weight * rhs;
}

Renderer ::~ Renderer ( void ) {
	delete camera ;
	delete heightMap ;
	delete quad ;
	delete reflectShader ;
	delete skyboxShader ;
	delete lightShader ;
	delete sunShader;
	delete particleShader;
	delete textureShader;
	delete root;
	Island::DeleteSphere();
	Island::DeleteHell();
	delete sunlight ;
	delete ghostlight;
	delete emitter;
	delete snowMachine;
	delete basicFont;
	currentShader = 0;
}

void Renderer :: UpdateScene (float msec ) {
	fps = 1000.0f / msec;
	
	emitter->Update(msec);
	cloudMachine->Update(msec);

	//increase amount of lying snow if snowing, else start melting it
	if(isSnowing){
		snowMachine->Update(msec);
		snowAmount += 0.001;
	}else
		snowAmount -= 0.001;

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
		ghostlight->SetRadius(400);
		//skyColourMod = Vector3(0.3f, 0.3f, 0.4f);
	}else if(sunlight->GetRadius() < 5000){
		//skyColourMod = Vector3(1,1,1);
		isNight = false;
		ghostlight->SetRadius(0);
	}
	int radius = lerp(clamp(sunlight->GetPosition().y, 0, 550)/550,0,7000);
	sunlight->SetRadius(radius);
}

void Renderer :: RenderScene () {
	glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );	

	DrawSkybox ();
	DrawHeightmap ();
	DrawWater ();
	DrawCloud();

	if(isSnowing)
		DrawSnow();

	if(isNight)
		DrawParticles();		

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
		root->hellNode->SetColour(Vector4(1,0.8f,0.8f,0.4f));
		glUniform1i ( glGetUniformLocation ( sunShader -> GetProgram (),"diffuseTex"), 1);
		DrawNode(root->hellNode);
	}

	stringstream ss;
	ss << dayLengthSeconds;
	string dayLength = "Length of Day: " + ss.str() + " seconds";
	DrawText(dayLength, Vector3(0,0,0), 16.0f, false);

	ss.str("");
	ss.clear();

	ss << fps;
	DrawText("FPS: " + ss.str(), Vector3(width-200,0,0), 16.0f, false);
	DrawText("Start/Stop Snow: T/Y", Vector3(0,height - 60,0), 16.0f, false);
	DrawText("Slow/Speed Up Time: O/P", Vector3(0,height - 30,0), 16.0f, false);

	glUseProgram(0);

	SwapBuffers ();	
}

void Renderer :: DrawParticles(){
	modelMatrix = Matrix4 :: Translation (Vector3 (1600,450,1800)) * Matrix4::Scale(Vector3(5000,5000,5000));

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	SetCurrentShader(particleShader);
	glUseProgram(particleShader->GetProgram());
	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 0);

	emitter->SetParticleSize(3.0f);
	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(10.0f);
	emitter->SetParticleLifetime(1000.0f);
	emitter->SetParticleSpeed(0.00003f);
	emitter->setColour(Vector4(1,0,0,1));
	//emitter->SetDirection(Vector3(0,1,0));
	UpdateShaderMatrices();

	emitter->Draw();

	glUseProgram(0);
	glDisable(GL_BLEND);
}

void Renderer :: DrawSnow(){
	modelMatrix = Matrix4 :: Translation (Vector3 (2000,1700,1700)) * Matrix4::Scale(Vector3(5000,5000,5000));

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	SetCurrentShader(particleShader);
	glUseProgram(particleShader->GetProgram());
	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 0);

	snowMachine->SetParticleSize(3.0f);
	SetShaderParticleSize(snowMachine->GetParticleSize());
	snowMachine->SetParticleVariance(1.0f);
	snowMachine->SetLaunchParticles(16.0f);
	snowMachine->SetParticleLifetime(10000.0f);
	snowMachine->SetParticleSpeed(0.00005f);
	snowMachine->setColour(Vector4(1,1,1,1));
	snowMachine->SetDirection(Vector3(0,-1,0));
	UpdateShaderMatrices();

	snowMachine->Draw();

	glUseProgram(0);
	glDisable(GL_BLEND);
}

void Renderer :: DrawCloud(){
	modelMatrix = Matrix4 :: Translation (Vector3 (-1000,2500,1700)) * Matrix4::Scale(Vector3(5000,5000,5000));

	glEnable ( GL_BLEND );
	//glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	SetCurrentShader(particleShader);
	glUseProgram(particleShader->GetProgram());
	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(cloudMachine->GetParticleSize());
	cloudMachine->SetDirection(Vector3(1.0f,0.0f,0.0f));
	cloudMachine->SetParticleSize(1000.0f);
	cloudMachine->SetParticleVariance(0.300);
	cloudMachine->SetLaunchParticles(1.0f);
	cloudMachine->SetParticleLifetime(300000.0f);
	cloudMachine->SetParticleSpeed(0.00002f);
	cloudMachine->SetParticleRate(20000.0f);
	cloudMachine->setColour(Vector4(0.9,0.9,0.9,0.6));
	UpdateShaderMatrices();

	cloudMachine->Draw();

	glUseProgram(0);
	glDisable(GL_BLEND);
}

void Renderer :: DrawSkybox () {
	glDepthMask ( GL_FALSE );
	SetCurrentShader ( skyboxShader );
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"colourMod"), 1, (float*)&skyColourMod); 
	glUniform1f(glGetUniformLocation ( currentShader -> GetProgram () ,"sunHeight"), sunlight->GetPosition().y);
	UpdateShaderMatrices ();
	quad -> Draw ();

	glUseProgram (0);
	glDepthMask ( GL_TRUE );
}

void Renderer :: DrawHeightmap () {

	SetCurrentShader ( lightShader );
	SetShaderLight (* sunlight );	if(isNight)		SetShaderLight(* ghostlight);	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ());
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMax"), 1, (float*)&maxAmbient); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMin"), 1, (float*)&minAmbient); 
	glUniform1f(glGetUniformLocation ( currentShader -> GetProgram () ,"sunHeight"), sunlight->GetPosition().y);
	glUniform1f(glGetUniformLocation ( currentShader -> GetProgram () ,"snowAmount"), snowAmount);

	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTexLower") , 0);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"bumpTexLower") , 1);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"diffuseTexUpper") , 2);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"bumpTexUpper") , 3);
	glUniform1i ( glGetUniformLocation ( currentShader -> GetProgram () ,"snowTexture") , 5);

	modelMatrix.ToIdentity ();
	textureMatrix.ToIdentity ();

	UpdateShaderMatrices ();

	heightMap -> Draw ();

	glUseProgram (0);
}

void Renderer :: DrawWater () {

	SetCurrentShader ( reflectShader );		SetShaderLight (* sunlight );

	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"cameraPos") ,1 ,(float *)& camera -> GetPosition ()); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMax"), 1, (float*)&maxAmbient); 
	glUniform3fv ( glGetUniformLocation ( currentShader -> GetProgram () ,"ambientMin"), 1, (float*)&minAmbient); 
	glUniform1i(glGetUniformLocation ( currentShader -> GetProgram () ,"isNight"), isNight);
	glUniform1f(glGetUniformLocation ( currentShader -> GetProgram () ,"sunHeight"), sunlight->GetPosition().y);
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
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
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
	glDisable(GL_BLEND);
}

void	Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::increaseDayLength(){
	dayLengthSeconds += 10;
}

void Renderer::decreaseDayLength(){
	if(dayLengthSeconds - 10 > 0)
		dayLengthSeconds -= 10;
}

void Renderer::startSnow(){
	isSnowing = true;
}

void Renderer::stopSnow(){
	isSnowing = false;
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)	{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glUseProgram(textShader->GetProgram());
	SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);

	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text,*basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if(perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	}
	else{	
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x,height-position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
		textureMatrix.ToIdentity();
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();
	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	delete mesh; //Once it's drawn, we don't need it anymore!
	glDisable(GL_BLEND);
}