#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
VERTEX_BUFFER , COLOUR_BUFFER , TEXTURE_BUFFER ,NORMAL_BUFFER , TANGENT_BUFFER , INDEX_BUFFER , MAX_BUFFER
};
class Mesh {
public :

	Mesh ( void );
	~ Mesh ( void );

	void SetBumpMap ( GLuint tex ) { bumpTexture = tex ;}
	GLuint GetBumpMap () { return bumpTexture ;}

	virtual void Draw ();
	static Mesh * GenerateTriangle ();

	void SetTexture ( GLuint tex ) { texture = tex ;}
	GLuint GetTexture () { return texture ;}

	static Mesh* GenerateQuad();

protected :
	void BufferData ();

	void GenerateTangents ();
	Vector3 GenerateTangent ( const Vector3 &a , const Vector3 &b ,	const Vector3 &c , const Vector2 & ta ,	const Vector2 & tb , const Vector2 & tc );
	Vector3 * tangents ;
	GLuint bumpTexture ;

	GLuint arrayObject ;
	GLuint bufferObject [ MAX_BUFFER ]; //max buffer = 2
	GLuint numVertices ;
	GLuint type ;

	Vector3 * vertices ;
	Vector4 * colours ;

	GLuint texture ;
	Vector2 * textureCoords ;

	GLuint numIndices ;
	unsigned int * indices ;

	void GenerateNormals ();	Vector3 * normals ;

};