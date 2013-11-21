#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "..\..\nclgl\OBJMesh.h"
#include "../../nclgl/SceneNode.h"
#include "ParticleEmitter.h"
#include "Island.h";

class Renderer : public OGLRenderer {
public :
	Renderer ( Window & parent );
	 virtual ~ Renderer ( void );

	 virtual void RenderScene ();
	 virtual void UpdateScene ( float msec );
	 void startSnow();
	 void stopSnow();
	 void increaseDayLength();
	 void decreaseDayLength();

 protected :
	 void DrawHeightmap ();
	 void DrawWater ();
	 void DrawSkybox ();
	 void DrawParticles();
	 void DrawSnow();

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * sunShader;
	 Shader * particleShader;

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
	 bool isSnowing;
	 float snowAmount;

	 void	SetShaderParticleSize(float f);	
	 ParticleEmitter* emitter;
	 ParticleEmitter* snowMachine;
 };