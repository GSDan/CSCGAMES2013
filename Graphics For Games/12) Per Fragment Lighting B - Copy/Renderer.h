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
	 Shader * sunShader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * sunlight ;
	 Light * ghostlight;
	 Camera * camera ;

	 GLuint dayCubeMap ;
	 GLuint nightCubeMap;

	 float waterRotate ;
	 Vector3 resolution;

	 int dayLengthSeconds;
	 float initTimer;

	 Vector3 minAmbient;
	 Vector3 maxAmbient;
	 Vector3 skyColourMod;

	 void DrawNode ( SceneNode *n);
	 void DrawMD5Node (MD5Node *n);
	 Island* root ;

	 bool isNight;

 };