#include "Mesh.h"
#include <stdio.h>

Mesh::Mesh(void)
{
	for(int i = 0 ; i < MAX_BUFFER ; i++)
	{
		bufferObject[i] = 0;
	}

	// create a name for vertex array object
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices    = NULL;
	colours     = NULL;
	type		= GL_TRIANGLES;
}

Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;
}


Mesh* Mesh::GenerateTriangle()
{
	Mesh *m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);


	m->colours = new Vector4[m->numVertices]; 
	// yellow
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f , 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f , 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f , 1.0f);






	m->BufferData();
	return m;
}

void Mesh::updateTriangleColours(float* ptr)
{



	this->colours[0] = Vector4(-0.5f, 0.0f, 2.0f,1.0f);
	this->colours[1] = Vector4(-0.5f, 0.0f, 2.0f,1.0f);
	this->colours[2] = Vector4(-0.5f, 0.0f, 2.0f,1.0f);






}

void Mesh::BufferData()
{
	GLvoid* ColorBuffer;
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);



	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	/** copies data into graphics memory
	*@param numVertices*size(Vector3) size of data copying 9 floats
	*@param vertices	a pointer to the start of the data to copy
	**/
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3), vertices, GL_DYNAMIC_DRAW);
	
	//GLfloat* data;


	//float *ptr = (float *) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_WRITE );
	//if(ptr)
	//{
	//	for(int i = 0; i < 8; ++i )
	//	{
	//		ptr[3*i+1] *= 2.0;  /* Modify Z values */
	//	}
	//	glUnmapBuffer(GL_ARRAY_BUFFER);

	//}



	//tells open gl that the vertex attribute has 3 float components per vertex
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0 , 0);

	//enables it
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if(colours)
	{
		glGenBuffers(1 , &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE,0,0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
}


void Mesh::Draw()
{
	glBindVertexArray(arrayObject);
	glDrawArrays(type, 0, numVertices);
	glBindVertexArray(0);

}
