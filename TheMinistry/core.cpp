#include"core.h"
#include"filecore.h"

#pragma comment(lib, "OpenGL32")
#pragma comment(lib, "libs/glfw3")
#pragma comment(lib, "libs/glew32s")

namespace{syscore sys; glcore glc; filecore fc;}
syscore& syscore::core(){return sys;}
glcore& glcore::core(){return glc;}
filecore& filecore::core(){return fc;}
rendernode::rendernode() : 
	render([](){return true;}),
	laterender([](){return false;}) {}

triggernode::triggernode() :
		controltrigger([](std::string){return true;}),
		handletrigger([](std::string){return false;}) {}

eventnode::eventnode() : 
		controlmousekey([](int,int,int){return true;}),
		controlmousepos([](double, double){return true;}),
		controlkey([](int){return true;}),
		controlsyskey([](int,int,int){return true;}),

		handlemousekey([](int,int,int){return false;}),
		handlemousepos([](double, double){return false;}),
		handlekey([](int){return false;}),
		handlesyskey([](int,int,int){return false;}) {}

void syscore::init(bool fullscreen, int width, int height){
	this->fullscreen = fullscreen;

	verifye(glfwInit(), GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	window = glfwCreateWindow(width, height, "The Ministry", fullscreen?glfwGetPrimaryMonitor():0, 0);

	glfwGetWindowSize(window, &this->w, &this->h);

	glfwSwapInterval(1);
	glfwSetKeyCallback(window, [](GLFWwindow*, int key, int scancode, int mode ,int mods){syscore().syskey(key, mode, mods);});
	glfwSetCharCallback(window, [](GLFWwindow*, unsigned int ch){syscore().key(ch);});
	glfwSetCursorPosCallback(window, [](GLFWwindow*, double x, double y){syscore().mousepos(x, y);});
	glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int mode, int mods){syscore().mousekey(button, mode, mods);});
}
syscore::syscore() : eventroot(0), renderroot(0), triggerroot(0) {}
int syscore::mainloop(){
		
	glfwMakeContextCurrent(window);
		
	glewExperimental = true;
	verifye(glewInit(), GLEW_OK);

	unsigned long realtime = static_cast<unsigned long>(glfwGetTime()*1000.0);
	unsigned long systime = realtime;
		
	trigger("startup");
		
	while(!glfwWindowShouldClose(window)){
		realtime = static_cast<unsigned long>(glfwGetTime()*1000.0);
		while(systime+sysframelen() <= realtime){
			trigger("step");
			systime += sysframelen();
		}
		
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}