#include "Renderer.h"
#include "../../nclgl/MD5Node.h"


Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= new Camera(0,90.0f,Vector3(-180,60,0));

#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader   = new Shader("../../Shaders/skeletonVertex.glsl", "../../Shaders/TexturedFragment.glsl");
#else
	currentShader   = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
#endif

	hellData		= new MD5FileData("../../Meshes/hellknight.md5mesh");
	hellNode		= new MD5Node(*hellData);

	hellData->AddAnim("../../Meshes/idle2.md5anim");
	hellNode->PlayAnim("../../Meshes/idle2.md5anim");

	if(!currentShader->LinkProgram()) {
		return;
	}
	projMatrix = Matrix4::Perspective(1.0f,10000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer(void)	{
	delete camera;
	delete hellData;
	delete hellNode;
}

 void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec); 
	viewMatrix		= camera->BuildViewMatrix();
	hellNode->Update(msec);
}

void Renderer::RenderScene()	{
	glClearColor(1,1,1,1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

#ifdef MD5_USE_HARDWARE_SKINNING
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "weightTex"), MD5_WEIGHT_TEXNUM);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "transformTex"), MD5_TRANSFORM_TEXNUM);
#endif

	UpdateShaderMatrices();

	hellNode->Draw(*this);

	glUseProgram(0);
	SwapBuffers();
}