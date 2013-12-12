#include "Renderer.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	

	camera = NULL;

	root = new SceneNode();

	simpleShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	textShader = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
	
	if(!simpleShader->LinkProgram() || !textShader->LinkProgram()){
		return;
	}

	 //setup basic font
    basicFont = new Font(SOIL_load_OGL_texture("../../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	instance = this;

	init = true;
}

Renderer::~Renderer(void)	{
	delete root;
	delete simpleShader;

	currentShader = NULL;
}

void Renderer::UpdateScene(float msec)	{
	fps = 1000.0f / msec;
	if(camera) {
		camera->UpdateCamera(msec); 
	}
	root->Update(msec);
}

void Renderer::RenderScene(int& objectSize)	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if(camera) {
		SetCurrentShader(simpleShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), 1);

		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix		= camera->BuildViewMatrix();
		projMatrix		= Matrix4::Perspective(1.0f,10000.0f,(float)width / (float) height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		UpdateShaderMatrices();

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}

	stringstream ss;
	ss << fps;
    DrawText("FPS: " + ss.str(), Vector3(width-150,0,0), 20.0f, false);

	ss.str("");
    ss.clear();

	ss << objectSize;
    DrawText("Size: " + ss.str(), Vector3(0,0,0), 16.0f, false);

	glUseProgram(0);
	SwapBuffers();
}

void	Renderer::DrawNode(SceneNode*n)	{
	if(n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),	1,false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),1,(float*)&n->GetColour());

		n->Draw(*this);
	}
}

void	Renderer::BuildNodeLists(SceneNode* from)	{
	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction,direction));

	if(frameFrustum.InsideFrustum(*from)) {
		if(from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else{
			nodeList.push_back(from);
		}
	}

	for(vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()	 {
	for(vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i ) {
		DrawNode((*i));
	}

	for(vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i ) {
		DrawNode((*i));
	}
}

void	Renderer::SortNodeLists()	{
	std::sort(transparentNodeList.begin(),	transparentNodeList.end(),	SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),				nodeList.end(),				SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	{
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::SetCamera(Camera*c) {
	camera = c;
}

void	Renderer::AddNode(SceneNode* n) {
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n) {
	//cout<<"UIHUIH"; ?????
	root->RemoveChild(n);
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)        {
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