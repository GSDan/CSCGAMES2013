#include "PhysicsNode.h"

PhysicsNode::PhysicsNode(void)	{
	target = NULL;
	isSphere = false;
	isCube = false;
}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position) {
	m_orientation	= orientation;
	m_position		= position;
	isSphere = false;
	isCube = false;
}

PhysicsNode::~PhysicsNode(void)	{

}

//You will perform your per-object physics integration, here!
//I've added in a bit that will set the transform of the
//graphical representation of this object, too.
void	PhysicsNode::Update(float msec) {

	Vector3 finalAcceleration = m_force * m_invMass + m_gravity;
	
	//only do velocity calculations if the velocity/acceleration vectors have some substance, else set velocity to 0
	if(abs(m_linearVelocity.x) + abs(finalAcceleration.x) > 0.1f || abs(m_linearVelocity.y) + abs(finalAcceleration.y) > 0.1f 
		|| abs(m_linearVelocity.z) + abs(finalAcceleration.z) > 0.1f ){

		//apply air resistance, positive or negative depending on the current direction.
		//make sure to only apply if the current acceleration > 0
		if(finalAcceleration.x != 0)(finalAcceleration.x > 0)?(finalAcceleration.x -= m_airResistance):(finalAcceleration.x += m_airResistance);
		if(finalAcceleration.y != 0)(finalAcceleration.y > 0)?(finalAcceleration.y -= m_airResistance):(finalAcceleration.y += m_airResistance);
		if(finalAcceleration.z != 0)(finalAcceleration.z > 0)?(finalAcceleration.z -= m_airResistance):(finalAcceleration.z += m_airResistance);

		//semi-implicit Euler Integration
		//next velocity = this velocity + this acceleration * change in time
		//next position = this position + next velocity * change in time
		m_linearVelocity = m_linearVelocity + finalAcceleration * msec;
		m_position = m_position + m_linearVelocity * msec;

	}else{
		m_linearVelocity = Vector3(0,0,0);
	}

	//do cube and sphere rotation physics here
	Vector3 angAccel = m_invInertia * m_torque;
	m_angularVelocity = m_angularVelocity + angAccel * msec;
	m_orientation = m_orientation +  (m_orientation * m_angularVelocity);



	if(target) {
		target->SetTransform(BuildTransform());
	}

	//reset The Force vector
	m_force = Vector3(0,0,0);
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
 ; \  `.; ;                    : : "+. ; 
 :  ;   ; ;                    : ;  : \: 
 ;  :   ; :                    ;:   ;  : 
: \  ;  :  ;                  : ;  /  :: 
;  ; :   ; :                  ;   :   ;: 
:  :  ;  :  ;                : :  ;  : ; 
;\    :   ; :                ; ;     ; ; 
: `."-;   :  ;              :  ;    /  ; 
 ;    -:   ; :              ;  : .-"   : 
 :\     \  :  ;            : \.-"      : 
  ;`.    \  ; :            ;.'_..--  / ; 
  :  "-.  "-:  ;          :/."      .'  :
   \         \ :          ;/  __        :
    \       .-`.\        /t-""  ":-+.   :
     `.  .-"    `l    __/ /`. :  ; ; \  ;
       \   .-" .-"-.-"  .' .'j \  /   ;/ 
        \ / .-"   /.     .'.' ;_:'    ;  
         :-""-.`./-.'     /    `.___.'   
               \ `t  ._  /            
                "-.t-._:'                

				?*
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