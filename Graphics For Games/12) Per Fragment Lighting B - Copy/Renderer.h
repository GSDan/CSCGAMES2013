#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public :
	Renderer ( Window & parent );
	 virtual ~ Renderer ( void );

	 virtual void RenderScene ();
	 virtual void UpdateScene ( float msec );

 protected :
	 void DrawHeightmap (Vector3 ambientColour);
	 void DrawWater (Vector3 ambientColour);
	 void DrawSkybox ();
	 void DrawSun(float msec);

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * basicShader;
	 Shader * md5Shader;
	 Shader * sceneShader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * light ;
	 Camera * camera ;

	 GLuint cubeMap ;

	 float waterRotate ;
	 Vector3 resolution;

	 int time;

 };