#pragma once
#include <iostream>
#include "..\..\nclgl\OGLRenderer.h"
using namespace std;
enum MeshBuffer{
	VERTEX_BUFFER, COLOUR_BUFFER, MAX_BUFFER
};

class Mesh{
public:
	
	//Initialise all Vertex Buffer Objects to 0  
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	void updateTriangleColours(float *ptr);
protected:

	// copies the mesh vertex data into graphics memory
	void BufferData();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;
};

