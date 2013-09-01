#pragma once
#include<set>
#include<map>
#include<algorithm>
#include<unordered_map>
#include<functional>

//Note: all graph implementations allow for multiple disjoint graph fulfilling the given criteria.

template<typename T>
class digraph;
template<typename T>
class acyclicdigraph;
template<typename T>
class tree;

template<typename T>
class node{
	bool visited;
	std::set<T*> outgoing;
	std::set<T*> incoming;
	std::string name;

	digraph<T> *container;

	T *thiz;
	
	node(const node<T> &rhs);
	node<T>& operator=(const node<T> &rhs);

public: 

	std::set<T*> outedges(){
		return outgoing;
	}

	std::set<T*> inedges(){
		return incoming;
	}

	void setup(digraph<T> *container, std::string name){
		this->container = container;
		thiz = dynamic_cast<T*>(this);
		this->name = name;
	}

	std::string id(){return name;}

	//Fails if edge already exists.
	bool edgeto(T *n){
		if(n->incoming.find(thiz) != n->incoming.end() || outgoing.find(n) != outgoing.end()) return false;
		
		n->incoming.insert(thiz);
		outgoing.insert(n);

		return true;
	}

	//Remove a connection from this to n.
	void orphan(T *n){
		n->incoming.erase(thiz);
		outgoing.erase(n);
	}
	
	//Reverse visit. Follow all edges backwards.
	bool rvisit(std::function<bool(T*)> n, std::function<bool(T*)> pn){
		if(visited) return false;
		visited = true;
		bool abort = false;
		if(n(thiz)){
			for_each(incoming.begin(), incoming.end(), [&](T *c){
				if(!abort) abort = c->rvisit(n, pn);
			});
		}
		return abort || pn(thiz);
	}

	//Cleanup step for reverse visit.
	void rclean(){
		if(!visited) return;
		visited = false;
		for_each(incoming.begin(), incoming.end(), [&](T *c){c->rclean();});
	}

	//Logical visit strategy.
	//n->true means go into children, pn->true means event handled, abort.
	bool visit(std::function<bool(T*)> n, std::function<bool(T*)> pn){
		if(visited) return false;
		visited = true;
		bool abort = false;
		if(n(thiz)){
			for_each(outgoing.begin(), outgoing.end(), [&](T *c){
				if(!abort) abort = c->visit(n, pn);
			});
		}
		return abort || pn(thiz);
	}

	//Standard cleanup step.
	void clean(){
		if(!visited) return;
		visited = false;
		for_each(outgoing.begin(), outgoing.end(), [&](T *c){c->clean();});
	}

	node() : visited(false) {}
	virtual ~node();
};

#define graphvisit(tree, type, pre, post, root) tree.visit([&](type *n) -> bool pre, [&](type *n) -> bool post, root)
#define nstruct(f) struct f : public node<f>

class genericnode : public node<genericnode> {};

//Generic directed graph
template<typename T>
class digraph{
	std::vector<T *> nodes;
	std::vector<std::pair<T*,T*>> edges;
	std::map<std::string, T*> names;

	friend node<T>;

	digraph(const digraph<T> &rhs);
	digraph<T>& operator=(const digraph<T> &rhs);
public:

	std::function<void(T*)> onNodeAdded;
	std::function<void(T*, T*)> onEdgeAdded;
	std::function<void(T*)> onNodeRemoved;
	std::function<void(T*, T*)> onEdgeRemoved;

	std::vector<T *> nodelist() const {
		return nodes;
	}

	typedef T graphnode;

	digraph(){
		onNodeAdded = [](T*){};
		onEdgeAdded = [](T*, T*){};
		onNodeRemoved = [](T*){};
		onEdgeRemoved = [](T*, T*){};
	}
	virtual ~digraph() {
		//Clean removal of all nodes.
		while(nodes.size()) delete *nodes.begin();
	};
	virtual void visit(std::function<bool(T*)> n, std::function<bool(T*)> pn, T *start = 0) {

		//a generic graph does not have a logical starting point for a visit, 
		//so visit all nodes randomly if not given.
		if(start == 0) 
			for_each(nodes.begin(),nodes.end(), [&](T *a){n(a), pn(a);}); 
		else {
			start->visit(n, pn);
			start->clean();
		}
	}

	virtual bool validate(T *n, T *c){

		//A connection is always valid in a digraph.
		return true;
	}

	//Build an edge from n to c.
	virtual bool connect(T *n, T *c){
		if(n == 0 || c == 0) return false;
		if(validate(n, c) && n->edgeto(c)) {

			//Storing a copy of the edge list for quick traversal.
			edges.push_back(std::make_pair(n,c));
			onEdgeAdded(n,c);
		}
		return false;
	}

	virtual T* create(std::string name = ""){

		//enforce unique names.
		if(name != "" && names.find(name) != names.end()) return 0;

		auto v = new T();
		nodes.push_back(v);
		v->setup(this, name);
		if(name != "")
			names[name] = v;
		onNodeAdded(v);
		return v;
	}

	virtual T* find(const std::string name) const {
		auto n = names.find(name);
		if(n == names.end()) return 0;
		return n->second;
	}

	virtual void disconnect(T *n, T *c){
		for(auto i=edges.begin(); i!=edges.end(); ++i){
			auto d = *i;
			if(d.first == n && d.second == c){
				edges.erase(i);
				n->orphan(c);
				onEdgeRemoved(n,c);
				break;
			}
		}
	}
};

template<typename T>
node<T>::~node(){
	while(incoming.size() > 0){container->disconnect(*incoming.begin(), thiz);}
	while(outgoing.size() > 0){container->disconnect(thiz, *outgoing.begin());}
	container->nodes.erase(std::find(container->nodes.begin(), container->nodes.end(), thiz));
	if(name != "") container->names.erase(name);
	container->onNodeRemoved(thiz);
}

//A non-directed graph. i.e. all edges go both ways.
template<typename T>
class graph : public digraph<T>{
public:
	virtual bool connect(T *n, T *c){

		//Setup both connections always.
		return digraph::connect(n,c) && digraph::connect(c,n);
	}

	virtual void disconnect(T *n, T *c){
		digraph::disconnect(n,c);
		digraph::disconnect(c,n);
	}
};

//An acyclic version of a directed graph.
template<typename T>
class acyclicdigraph : public digraph<T>{
public:
	virtual bool validate(T *n, T *c){
		bool ok = true;

		//Validate all incoming nodes to disallow cycles
		n->rvisit([&](T *p) -> bool{
			if(p == c) ok = false;
			return ok;
		}, [&](T *p){return false;});
		n->rclean();

		return ok;
	}
};

//A tree is a specialization of an acyclic digraph, where no node can have more than one parent.
template<typename T>
class tree : public acyclicdigraph<T>{
public:
	virtual bool validate(T *n, T *c){
		//Run digraph validation and additionally ensure child has no existing parents
		return c->incoming.size() == 0 && digraph::validate(n, c);
	}
};