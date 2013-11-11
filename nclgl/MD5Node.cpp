#include "MD5Node.h"

MD5Node::MD5Node(const MD5FileData &ofType) : sourceData(ofType)	{
	currentAnim		 = NULL;
	frameTime		 = 0.0f;
	currentAnimFrame = 0;

	ofType.CloneSkeleton(currentSkeleton);

	mesh = ofType.GetRootMesh();
}

MD5Node::~MD5Node(void)	{

}

/*
This Overridden Update function will update the current animation frame used
by this particular mesh instance, by subtracting the incoming time value
by a float /frameTime/, which when less than 0, triggers a frame update. This
float is then reset to the framerate defined within the current animation. A
while is used instead of an if for the unusual occurance of a time value being
large enough that we could risk skipping over frames - generally this only 
happens when we hit a debug breakpoint.

*/
void	MD5Node::Update(float msec) {
	if(currentAnim) {
		frameTime -= msec;
		//Time to calculate which frame we're now on!
		while(frameTime < 0) {
			frameTime += 1000.0f / currentAnim->GetFrameRate();
			//all animations are assumed to be 'looping', so we use the modulo
			//operator to 'wrap around' if we go past the end of the anim
			currentAnimFrame = currentAnimFrame++%(currentAnim->GetNumFrames());
		}
		//Transform this particular node's skeleton to the right frame of
		//anim
		currentAnim->TransformSkeleton(currentSkeleton,currentAnimFrame-1);
	}
	//Call our base class update function, too! Doing so will presever the 
	//ability to build up the world matrices for every node. 
	SceneNode::Update(msec);
}

/*
Swaps the currently used animation of this MD5Mesh. 
*/
void	MD5Node::PlayAnim(std::string name)	{
/*
We want to reset all of the animation details
*/
	currentAnimFrame	= 0;
	frameTime			= 0.0f; 
	currentAnim			= sourceData.GetAnim(name);
}

void	MD5Node::Draw(const OGLRenderer &r) {
	MD5Mesh*m = (MD5Mesh*)mesh;

	/*
	I have added experimental support for performing skinning inside the vertex
	shader, in order for everyone to see one way of getting lots of data into
	'shader space'. To do so, I use an OpenGL feature called a Texture Buffer
	Object, which allows a VBO to be 'seen' as texture data inside a shader.

	I store the current skeleton state inside a VBO, and then bind this VBO
	as a TBO, allowing the vertex shader to see the current skeleton state.

	I also store the mesh weights and anchor transforms inside a TBO, too, so
	that the shader can access all of the data necessary to update the vertex
	to the correct local position. This is a slight change in method to the
	'use lots of uniforms' method outlined in the tutorial writeup, but it is
	still worth considering the uniform method, as the current consoles do not
	support the arbitrary data lookup method used by OpenGL TBOs...
	*/

#ifdef MD5_USE_HARDWARE_SKINNING
	sourceData.BindTextureBuffers();
	sourceData.UpdateTransformTBO(currentSkeleton);
#else 
	/*
	If we're doing 'software' skinning, then we need to make sure the global mesh
	data is in the correct position for this node before we draw it, which we do
	by calling the 'skin vertices' function of the MD5Mesh, passing it our node's
	current skeleton, which will have been updated in the Update function to be in
	the correct pose for the current frame of animation. 
	*/
	m->SkinVertices(currentSkeleton);
#endif
	//Finally, we draw the mesh, just like the base class Draw function...
	m->Draw();
}