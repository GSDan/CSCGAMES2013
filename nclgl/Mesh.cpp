#include "Mesh.h"

Mesh :: Mesh ( void ) {

	//max buffer = 2 in header
for( int i = 0; i < MAX_BUFFER ; ++ i ) {
	bufferObject [ i ] = 0;
	//value 0 is treated like null in OpenGL, so will be ignored
}
glGenVertexArrays (1 , & arrayObject );

numVertices = 0;
 vertices = NULL ;
 colours = NULL ;
 type = GL_TRIANGLES ;
}

Mesh ::~ Mesh ( void ) {
	glDeleteVertexArrays (1 , & arrayObject );
	glDeleteBuffers ( MAX_BUFFER , bufferObject );
	delete [] vertices ;
	delete [] colours ;
}

Mesh * Mesh :: GenerateTriangle () {
	Mesh * m = new Mesh ();
	m -> numVertices = 3;

	m -> vertices = new Vector3 [m -> numVertices ];
	m -> vertices [0] = Vector3 (0.0f, 0.5f , 0.0f );
	m -> vertices [1] = Vector3 (0.5f, -0.5f , 0.0f );
	m -> vertices [2] = Vector3 ( -0.5f, -0.5f , 0.0f );

	m -> colours = new Vector4 [m -> numVertices ];
	m -> colours [0] = Vector4 (1.0f , 0.0f , 0.0f ,1.0f );
	m -> colours [1] = Vector4 (0.0f , 1.0f , 0.0f ,1.0f );
	m -> colours [2] = Vector4 (0.0f , 0.0f , 1.0f ,1.0f );

	m -> BufferData ();
	return m ;
}

void Mesh :: BufferData () {
	glBindVertexArray ( arrayObject );
	glGenBuffers (1 , & bufferObject [ VERTEX_BUFFER ]);
	glBindBuffer ( GL_ARRAY_BUFFER , bufferObject [ VERTEX_BUFFER ]);
	glBufferData ( GL_ARRAY_BUFFER , numVertices * sizeof ( Vector3 ),	vertices , GL_STATIC_DRAW );
	glVertexAttribPointer ( VERTEX_BUFFER , 3 , GL_FLOAT , GL_FALSE , 0 , 0);
	glEnableVertexAttribArray ( VERTEX_BUFFER );
	if ( colours ) { // Just in case the data has no colour attribute ...
	glGenBuffers (1 , & bufferObject [ COLOUR_BUFFER ]);
	glBindBuffer ( GL_ARRAY_BUFFER , bufferObject [ COLOUR_BUFFER ]);
	glBufferData ( GL_ARRAY_BUFFER , numVertices * sizeof ( Vector4 ) ,
	colours , GL_STATIC_DRAW );
	glVertexAttribPointer ( COLOUR_BUFFER , 4 , GL_FLOAT , GL_FALSE ,0 ,0);
	glEnableVertexAttribArray ( COLOUR_BUFFER );
	}
	glBindVertexArray (0);
}

void Mesh :: Draw () {
	glBindVertexArray ( arrayObject );
	glDrawArrays ( type , 0 , numVertices );
	glBindVertexArray (0);
}
