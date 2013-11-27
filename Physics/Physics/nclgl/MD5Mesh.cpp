#include "MD5Mesh.h"

MD5Mesh::MD5Mesh(const MD5FileData&t) :  type(t) {
#ifdef MD5_USE_HARDWARE_SKINNING
	weightObject = 0;
	weights		 = NULL;
#endif
}

MD5Mesh::~MD5Mesh(void)	{
#ifdef MD5_USE_HARDWARE_SKINNING
	delete weights;
	glDeleteBuffers(1, &weightObject);
#endif
}

///*
//Draws the current MD5Mesh. The handy thing about overloaded virtual functions
//is that they can still run the code they have 'overridden', by calling the 
//parent class function as you would a static function. So all of the new stuff
//you've been building up in the Mesh class as the tutorials go on, will 
//automatically be used by this overloaded function. Once 'this' has been drawn,
//all of the children of 'this' will be drawn
//*/

void MD5Mesh::Draw() {
	GL_BREAKPOINT;
	if(numVertices == 0) {
		//Assume that this mesh is actually our 'root' node
		//so set up the shader with our TBOs
#ifdef MD5_USE_HARDWARE_SKINNING
		type.BindTextureBuffers();
#endif

		for(unsigned int i = 0; i < children.size(); ++i) {
			children[i]->Draw();
		}
	}
	Mesh::Draw();
};

/*
In my experimental 'hardware' implementation of vertex skinning via a
vertex shader, I store the number of weight elements, and the starting
weight element as a vec2 additional vertex attribute - doing so gives
each vertex 'unlimited' weights, unlike the limited number of weights
we can use when making each weight an attribute. 

Note how I use the MAX_BUFFER macro to always make sure that the 
extra vertex attribute is always one more than whatever vertex attributes
we've made during the module are! Examine the vertex shader to see how
the shader can access this vertex attribute without us having to modify the
shader class "SetDefaultAttributes" function...

*/
#ifdef MD5_USE_HARDWARE_SKINNING
void MD5Mesh::BufferExtraData() {
	glBindVertexArray(arrayObject);

	glGenBuffers(1, &weightObject);
	glBindBuffer(GL_ARRAY_BUFFER, weightObject);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector2), weights, GL_STATIC_DRAW);
	glVertexAttribPointer(MAX_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(MAX_BUFFER);

	glBindVertexArray(0);
}
#endif

///*
//Skins each vertex by its weightings, producing a final skinned mesh in the passed in
//skeleton pose. 
//*/
void	MD5Mesh::SkinVertices(const MD5Skeleton &skel) {
	//For each submesh, we want to transform a position for each vertex
	for(unsigned int i = 0; i < type.numSubMeshes; ++i) {
		MD5SubMesh& subMesh = type.subMeshes[i];	//Get a reference to the current submesh
		/*
		Each MD5SubMesh targets a Mesh's data. The first submesh will target 'this', 
		while subsequent meshes will target the children of 'this'
		*/
		MD5Mesh*target		= (MD5Mesh*)children.at(i);

		/*
		For each vertex in the submesh, we want to build up a final position, taking
		into account the various weighting anchors used.
		*/
		for(int j = 0; j < subMesh.numverts; ++j) {
			//UV coords can be copied straight over to the Mesh textureCoord array
			target->textureCoords[j]   = subMesh.verts[j].texCoords;

			//And we should start off with a Vector of 0,0,0
			target->vertices[j].ToZero();

			/*
			Each vertex has a number of weights, determined by weightElements. The first
			of these weights will be in the submesh weights array, at position weightIndex.

			Each of these weights has a joint it is in relation to, and a weighting value,
			which determines how much influence the weight has on the final vertex position
			*/

			for(int k = 0; k < subMesh.verts[j].weightElements; ++k) {
				MD5Weight& weight	= subMesh.weights[subMesh.verts[j].weightIndex + k];
				MD5Joint& joint		= skel.joints[weight.jointIndex];

				/*
				We can then transform the weight position by the joint's world transform, and multiply
				the result by the weightvalue. Finally, we add this value to the vertex position, eventually
				building up a weighted vertex position.
				*/

				target->vertices[j] += ((joint.transform * weight.position) * weight.weightValue);				
			}
		}

		/*
		As our vertices have moved, normals and tangents must be regenerated!
		*/
#ifdef MD5_USE_NORMALS
		target->GenerateNormals();
#endif

#ifdef MD5_USE_TANGENTS_BUMPMAPS
		target->GenerateTangents();
#endif
		/*
		Finally, as our vertex attributes data has changed, we must rebuffer the data to 
		graphics memory.
		*/
		target->RebufferData();
	}
}


///*
//Rebuffers the vertex data on the graphics card. Now you know why we always keep hold of
//our vertex data in system memory! This function is actually entirely covered in the 
//skeletal animation tutorial text (unlike the other functions, which are kept as 
//pseudocode). This should be in the Mesh class really, as it's a useful function to have.
//It's worth pointing out that the glBufferSubData function never allocates new memory
//on the graphics card!
//*/
void MD5Mesh::RebufferData()	{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)vertices);

	if(textureCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector2), (void*)textureCoords);
	}

	if (colours)	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector4), (void*)colours);
	}

#ifdef MD5_USE_NORMALS
	if(normals) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)normals);
	}
#endif

#ifdef MD5_USE_TANGENTS_BUMPMAPS
	if(tangents) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)tangents);
	}
#endif

	if(indices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numVertices*sizeof(unsigned int), (void*)indices);
	}
}


//
///*
//Draws the current skeleton for this MD5Mesh, using GL_LINES and GL_POINTS. Temporarily creates
//a VBO and VAO, puts the joint positions in it, and draws it. An example of a use for
//GL_STREAM_DRAW.
//
//*/
//void MD5Mesh::DrawSkeleton()	{	
//	//Temporary VAO and VBO
//	unsigned int skeletonArray;
//	unsigned int skeletonBuffer;
//
//	glGenVertexArrays(1, &skeletonArray);
//	glGenBuffers(1, &skeletonBuffer);
//
//	//Temporary chunk of memory to keep our joint positions in
//	Vector3*	 skeletonVertices = new Vector3[bindPose.numJoints*2];
//
//
//	/*
//	Now for each joint we're going to have a pair of vertices - one at the joint position,
//	and one at the joint's parent's position. This'll let us draw lines to show the skeletal
//	shape. There'll be a bit of overdraw, which could be avoided by using indices. but this way
//	is 'good enough'
//	*/
//	for(int i = 0; i < currentSkeleton.numJoints; ++i) {
//		skeletonVertices[i*2] = currentSkeleton.joints[i].transform.GetPositionVector();
//
//		if(currentSkeleton.joints[i].parent == -1) {	//No parent, but to keep this simple we'll copy the position again...
//			skeletonVertices[(i*2)+1] = currentSkeleton.joints[i].transform.GetPositionVector();;
//		}
//		else{
//			skeletonVertices[(i*2)+1] = currentSkeleton.joints[currentSkeleton.joints[i].parent].transform.GetPositionVector();;
//		}
//	}
//
//	//You should know what this all does by now, except we combine it with the draw operations in a single function
//	glBindVertexArray(skeletonArray);
//	glBindBuffer(GL_ARRAY_BUFFER, skeletonBuffer);
//	glBufferData(GL_ARRAY_BUFFER, bindPose.numJoints*sizeof(Vector3)*2, skeletonVertices, GL_STREAM_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
//	glEnableVertexAttribArray(0);
//
//	glBindVertexArray(skeletonArray);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	//Draws the array twice, once as points, and once as lines. glLineWidth may or may not actually do anything
//	//as it is deprecated functionality in OGL 3.2. 
//	glPointSize(5.0f);
//	glLineWidth(2.0f);
//	glDrawArrays(GL_POINTS, 0, bindPose.numJoints*2);	// draw Joints
//	glDrawArrays(GL_LINES,  0, bindPose.numJoints*2);	// draw Bones
//	glPointSize(1.0f);
//	glLineWidth(1.0f);
//
//	glBindVertexArray(0);
//
//	//Delete the VBO and VAO, and the heap memory we allocated earlier
//	glDeleteVertexArrays(1, &skeletonArray);
//	glDeleteBuffers(1, &skeletonBuffer);
//	delete[]skeletonVertices;
//}