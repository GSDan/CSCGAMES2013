#include "PhysicsNode.h"

PhysicsNode::PhysicsNode(void)	{
	target = NULL;
}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position) {
	m_orientation	= orientation;
	m_position		= position;
}

PhysicsNode::~PhysicsNode(void)	{

}

//You will perform your per-object physics integration, here!
//I've added in a bit that will set the transform of the
//graphical representation of this object, too.
void	PhysicsNode::Update(float msec) {

	Vector3 finalAcceleration = m_force * m_invMass + m_gravity;
	m_linearVelocity = m_linearVelocity * Vector3(0.99f,0.99f,0.99f);

	if(Vector3(abs(m_linearVelocity.x + finalAcceleration.x), 
				abs(m_linearVelocity.y + finalAcceleration.y), 
				abs(m_linearVelocity.z + finalAcceleration.z)) < Vector3(0.003f,0.003f,0.003f)){
		m_linearVelocity = Vector3(0,0,0);
	}
	
	//semi-implicit Euler Integration
	//next velocity = this velocity + this acceleration * change in time
	//next position = this position + next velocity * change in time
		
	m_linearVelocity = m_linearVelocity + finalAcceleration * msec;
	m_position = m_position + m_linearVelocity * msec;

	//do cube and sphere rotation physics here
	//m_torque = Vector3(0,1,0);
	//m_torque = Vector3::Cross(m_torqueDistance , m_torqueForce);
    m_angularVelocity += m_invInertia * (m_torque*msec);
	m_angularVelocity = m_angularVelocity * Vector3(0.95f,0.95f,0.95f);
    m_orientation = m_orientation + m_orientation*(m_angularVelocity*msec*0.5f);
    m_orientation.Normalise();



	if(target) {
		target->SetTransform(BuildTransform());
	}

	//reset The Force vectors
	m_force = Vector3(0,0,0);
	m_torqueForce = m_force;
/*
	                  ____                  
                _.' :  `._               
            .-.'`.  ;   .'`.-.           
   __      / : ___\ ;  /___ ; \      __  
 ,'_ ""--.:__;".-.";: :".-.":__;.--"" _`,
 :' `.t""--.. '<@.`;_  ',@>` ..--""j.' `;
      `:-.._J '-.-'L__ `-- ' L_..-;'     
        "-.__ ;  .-"  "-.  : __.-"       
            L ' /.------.\ ' J           
             "-.   "--"   .-"            
            __.l"-:_JL_;-";.__           
         .-j/'.;  ;""""  / .'\"-.        
       .' /:`. "-.:     .-" .';  `.      
    .-"  / ;  "-. "-..-" .-"  :    "-.   
 .+"-.  : :      "-.__.-"      ;-._   \  
				*/
}

/*
This function simply turns the orientation and position
of our physics node into a transformation matrix, suitable
for plugging into our Renderer!

It is cleaner to work with matrices when it comes to rendering,
as it is what shaders expect, and allow us to keep all of our
transforms together in a single construct. But when it comes to
physics processing and 'game-side' logic, it is much neater to
have seperate orientations and positions.

*/
Matrix4		PhysicsNode::BuildTransform() {
	Matrix4 m = m_orientation.ToMatrix();

	m.SetPositionVector(m_position);

	return m;
}

void PhysicsNode::calcCubeInvInertia(float size){
	float inertia = 1/12*m_mass*(2 * (size * size));
	float arr[16] = {inertia, 0, 0, 0, 
					 0, inertia, 0, 0,
					 0, 0, inertia, 0,
					 0, 0, 0, 1};
	m_invInertia = Matrix4(arr);
}