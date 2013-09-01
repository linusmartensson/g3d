#include"core.h"
#include"filecore.h"


template<typename T>
class uigraph{
	T *graph;
public:

	//Unallocated elements in the various buffers.
	std::set<unsigned int> freed;
	
	//Positions of rendered nodes.
	std::vector<GLfloat> positions;

	//Links between elements in the positions list for the purpose of drawing edges.
	std::vector<GLuint> edges;

	//Links between position vector and real nodes.
	std::vector<T::graphnode*> nslots;
	std::map<T::graphnode *n, unsigned int> slots;

	std::string positionbuffer;
	std::string edgeindexbuffer;

	void linkGraph(T *graph){
		this->graph = graph;
		auto &gl = glcore::core();

		positionbuffer = gl.genuid();
		edgeindexbuffer = gl.genuid();

		graph->onNodeAdded([&](T::graphnode *n){
			if(freed.size()){

			}
		});
		graph->onNodeRemoved([&](T::graphnode *n){
			
		});
		graph->onEdgeAdded([&](T::graphnode *n, T::graphnode *c){
			
		});
		graph->onEdgeRemoved([&](T::graphnode *n, T::graphnode *c){
			
		});
		//Build nodes from node list.
		//Build edges from edge list.

	}

	void render(){
		
	}
};


void setup(int argc, char** argv){
	//Note, OpenGL is not yet available at this point.

	auto &gl = glcore::core();

	auto &rt = syscore::core().rendertree;
	auto &tt = syscore::core().triggertree;
	auto &et = syscore::core().eventtree;

	auto rr = rt.create("root");
	auto tr = tt.create("root");
	auto er = et.create("root");

	syscore::core().renderroot = rr;
	syscore::core().triggerroot = tr;
	syscore::core().eventroot = er;

	digraph<genericnode> testgraph;
	filecore::core().loadGraph("test.graph", testgraph);

	rr->render = [&](){
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		return true;
	};

	tr->controltrigger = [&](std::string s){
		//Build loading list.
		return true;
	};

	tr->handletrigger = [&](std::string s){
		//Load
		return false;
	};

	uigraph<digraph<genericnode>> dig;

	auto gr = rt.create("graphrenderer");
	auto gt = tt.create("graphtriggers");
	auto ge = et.create("graphevents");
	rt.connect(rr, gr);
	tt.connect(tr, gt);
	et.connect(er, ge);

	gr->render = [&]() -> bool {
		dig.render();
		return true;
	};
}