#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include"Mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 12.0f
#define HEIGHTMAP_Z 12.0f
#define HEIGHTMAP_Y 5.0f
#define HEIGHTMAP_TEX_X 1.0f / 4.0f
#define HEIGHTMAP_TEX_Z 1.0f / 4.0f

class HeightMap : public Mesh {
public :
	HeightMap ( std :: string name );
	~ HeightMap ( void ){};

	void Draw();

	void SetTextureLower(GLuint tex ) { textureLower = tex ;}
	void SetTextureUpper(GLuint tex ) { textureUpper = tex ;}

	GLuint GetTextureLower () { return textureLower;}
	GLuint GetTextureUpper () { return textureUpper;}

	void SetBumpMapLower ( GLuint tex ) { bumpLower = tex ;}
	void SetBumpMapUpper ( GLuint tex ) { bumpUpper = tex ;}

	GLuint GetBumpMapLower () { return bumpLower;}
	GLuint GetBumpMapUpper () { return bumpUpper;}

protected:
	GLuint textureLower;
	GLuint textureUpper;

	GLuint bumpLower;
	GLuint bumpUpper;
	
};
