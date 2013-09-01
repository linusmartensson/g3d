#pragma once
#include"graph.h"
#include<iostream>

#define verifye(a, b) if(a != b) std::cout<<#a<<std::endl, exit(1);
#define verifyne(a, b) if(a == b) std::cout<<#a<<std::endl, exit(1);

#include<set>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<string>
#include<functional>

#include"glcore.h"

nstruct(rendernode){
	std::function<bool()> render;
	std::function<bool()> laterender;
	
	rendernode();
};

nstruct(triggernode){
	std::function<bool(std::string )> controltrigger;
	std::function<bool(std::string )> handletrigger;

	triggernode();
};

nstruct(eventnode){
	std::function<bool(int , int , int )> controlmousekey;
	std::function<bool(double , double )> controlmousepos;
	std::function<bool(unsigned int )> controlkey;
	std::function<bool(int, int , int)> controlsyskey;

	std::function<bool(int , int , int )> handlemousekey;
	std::function<bool(double , double )> handlemousepos;
	std::function<bool(unsigned int )> handlekey;
	std::function<bool(int, int , int)> handlesyskey;

	eventnode();
};

class syscore{

	int w, h;
	bool fullscreen;
	GLFWwindow *window;
	
	syscore(const syscore &rhs);
	syscore& operator=(const syscore &rhs);
public:


	syscore();

	//Core graphs in the system pipeline.
	acyclicdigraph<rendernode> rendertree;
	acyclicdigraph<eventnode> eventtree;
	acyclicdigraph<triggernode> triggertree;
	rendernode *renderroot;
	eventnode *eventroot;
	triggernode *triggerroot;

	static syscore& core();
	
	inline void trigger(std::string s){
		graphvisit(triggertree, triggernode, { return n->controltrigger(s); }, { return n->handletrigger(s); }, triggerroot);
	}
	inline void mousekey(int button, int mode, int mods){
		graphvisit(eventtree, eventnode, { return n->controlmousekey(button, mode, mods); }, { return n->handlemousekey(button,mode,mods); }, eventroot);
	}
	inline void mousepos(double x, double y){
		graphvisit(eventtree, eventnode, { return n->controlmousepos(x,y); }, { return n->handlemousepos(x,y); }, eventroot);
	}
	inline void key(unsigned int key){
		graphvisit(eventtree, eventnode, { return n->controlkey(key); }, { return n->handlekey(key); }, eventroot);
	}
	inline void syskey(int key, int mode, int mods){
		graphvisit(eventtree, eventnode, { return n->controlsyskey(key,mode,mods); }, { return n->handlesyskey(key,mode,mods); }, eventroot);
	}
	inline void render() {
		graphvisit(rendertree, rendernode, { 
			return n->render(); 
		}, { 
			return n->laterender(); 
		}, renderroot);
	}
	inline int width() const {
		return w;
	}
	inline int height() const {
		return h;
	}
	inline unsigned short sysframelen() const {
		return 1000/60;
	}

	void init(bool fullscreen = false, int width = 1280, int height = 720);
	
	int mainloop();
};

void setup(int argc, char **argv);