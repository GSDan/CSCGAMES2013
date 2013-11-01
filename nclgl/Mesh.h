#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	//initialised according to order, 0,1,2 etc
	VERTEX_BUFFER , COLOUR_BUFFER, TEXTURE_BUFFER, MAX_BUFFER
};
class Mesh {
public :
	Mesh ( void );
	~ Mesh ( void );

	virtual void Draw ();
	static Mesh * GenerateTriangle ();

	void SetTexture ( GLuint tex ) { texture = tex ;}
	GLuint GetTexture () { return texture ;}

	static Mesh* GenerateQuad();

protected :
	void BufferData ();

	GLuint arrayObject ;
	GLuint bufferObject [ MAX_BUFFER ]; //max buffer = 2
	GLuint numVertices ;
	GLuint type ;

	Vector3 * vertices ;
	Vector4 * colours ;

	GLuint texture ;
	Vector2 * textureCoords ;

};