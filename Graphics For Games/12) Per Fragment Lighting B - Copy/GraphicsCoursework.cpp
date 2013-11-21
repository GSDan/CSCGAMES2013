#pragma comment(lib,"nclgl.lib")

#include"../../nclGL/window.h"
//#include "../../nclgl/FPS.cpp"
#include"Renderer.h"

int main() {
	Window w("Island Coursework | Dan Richardson", 1280,720,false);
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_T)){
			//start snow
			renderer.startSnow();
		}else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_Y)){
			//stop snow
			renderer.stopSnow();
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_O)){
			//decrease day length
			renderer.decreaseDayLength();
		}else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)){
			//increase day length
			renderer.increaseDayLength();
		}

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
		//drawFPS();
	}
	return 0;
}

