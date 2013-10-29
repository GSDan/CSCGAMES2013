#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	//initialised according to order, 0,1,2 etc
VERTEX_BUFFER , COLOUR_BUFFER , MAX_BUFFER
};
class Mesh {
public :
	Mesh ( void );
	~ Mesh ( void );

	virtual void Draw ();
	static Mesh * GenerateTriangle ();

protected :
	void BufferData ();

	GLuint arrayObject ;
	GLuint bufferObject [ MAX_BUFFER ]; //max buffer = 2
	GLuint numVertices ;
	GLuint type ;

	Vector3 * vertices ;
	Vector4 * colours ;
};