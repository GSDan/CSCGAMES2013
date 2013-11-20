#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "..\..\nclgl\OBJMesh.h"
#include "../../nclgl/SceneNode.h"
#include "Island.h";

class Renderer : public OGLRenderer {
public :
	Renderer ( Window & parent );
	 virtual ~ Renderer ( void );

	 virtual void RenderScene ();
	 virtual void UpdateScene ( float msec );

 protected :
	 void DrawHeightmap ();
	 void DrawWater ();
	 void DrawSkybox ();

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * sceneShader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * sunlight ;
	 Camera * camera ;

	 GLuint cubeMap ;

	 float waterRotate ;
	 Vector3 resolution;

	 int dayLengthSeconds;

	 Vector3 minAmbient;
	 Vector3 maxAmbient;
	 Vector3 twiAmbient;

	 void DrawNode ( SceneNode *n);
	 Island* root ;

	 bool isNight;
 };