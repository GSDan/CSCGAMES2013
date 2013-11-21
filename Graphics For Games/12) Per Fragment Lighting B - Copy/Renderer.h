#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "..\..\nclgl\OBJMesh.h"
#include "../../nclgl/SceneNode.h"
#include "TextMesh.h"
#include <time.h> 
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

	 float fps;

	 void DrawHeightmap ();
	 void DrawWater ();
	 void DrawSkybox ();
	 void DrawParticles();
	 void DrawSnow();
	 void DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);

	 Font*	basicFont;

	 Shader * lightShader ;
	 Shader * reflectShader ;
	 Shader * skyboxShader ;
	 Shader * sunShader;
	 Shader * particleShader;
	 Shader * textShader;

	 HeightMap * heightMap ;
	 Mesh * quad ;

	 Light * sunlight ;
	 Light * ghostlight;
	 Camera * camera ;

	 TextMesh * txtMesh;
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