#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "..\..\nclgl\MD5Mesh.h"
#include "World.h"

class Renderer : public OGLRenderer {
public :
	Renderer ( Window & parent );
	 virtual ~ Renderer ( void );

	 virtual void RenderScene ();
	 virtual void UpdateScene ( float msec );

 protected :
	 void DrawHeightmap (Vector3 ambientColour);
	 void DrawWater ();
	 void DrawSkybox ();
	 void DrawSun(float msec);

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * basicShader;
	 Shader * md5Shader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * light ;
	 Camera * camera ;

	 GLuint cubeMap ;

	 float waterRotate ;
	 Vector3 resolution;

	 MD5FileData*	lampData;
	 MD5Node*		lampNode;

	 void DrawNode(SceneNode *n);
	 SceneNode* root;

 };