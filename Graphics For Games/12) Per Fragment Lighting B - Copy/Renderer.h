#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "..\..\nclgl\OBJMesh.h"

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
	 void updateSun();
	 void DrawSun(float msec);

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * md5Shader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * sunlight ;
	 Camera * camera ;

	 GLuint cubeMap ;

	 float waterRotate ;
	 Vector3 resolution;

	 float time;
	 Vector3 minAmbient;
	 Vector3 maxAmbient;
	 Vector3 twiAmbient;

	 static Mesh * sun;

 };