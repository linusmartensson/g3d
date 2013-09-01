#pragma once

#include<fstream>

template<typename T> 
void loadNode(T *node, std::istream &is){}

template<typename T>
void saveNode(T *node, std::ostream &os){}

class filecore {
	filecore(const filecore &rhs);
	filecore& operator=(const filecore &rhs);

public:
	filecore(){}

	static filecore& core();

	struct image{
		GLenum format, datatype;
		unsigned int width, height, size;
		void *data;
	};

	std::string loadText(std::string file){

		std::ifstream ifs(file, std::ios::ate | std::ios::binary);
		auto sz = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		auto *text = new char[static_cast<unsigned int>(sz)];
		ifs.read(text, sz);
		ifs.close();
		std::string ret;
		ret.append(text, static_cast<unsigned int>(sz));
		delete [] text;
		return ret;
	}

	template<typename T>
	void loadGraph(std::ifstream &ifs, T &graph){
		std::vector<T::graphnode*> nodes;
		std::map<T::graphnode*, std::vector<int>> edges;
		int count = 0;
		ifs>>count;
		while(ifs.good()){
			if(count-- == 0) break;
			int og = 0;
			int len = 0;
			ifs>>len;
			std::string name = "";
			if(len > 0 && ifs.good()){
				auto c = new char[len];
				ifs.read(c, len);
				name.append(c, len);
			}
			ifs>>og;
			if(ifs.good()){
				nodes.push_back(graph.create(name));
				auto &v = edges[nodes.back()];
				for(int i=0;i<og; ++i){
					int ed = 0;
					ifs>>ed;
					v.push_back(ed);
				}
				loadNode(nodes.back(), ifs);
			}
		}
		for(auto i = edges.begin(); i!=edges.end(); ++i){
			auto av = i;
			for_each(av->second.begin(), av->second.end(), [&](int c){
				graph.connect(av->first, nodes[c]);
			});
		};
	}

	template<typename T>
	void loadGraph(std::string file, T &graph){
		std::ifstream ifs(file);
		loadGraph(ifs, graph);
	}

	template<typename T>
	void saveGraph(std::ostream &ofs, T &graph){
		auto v = graph.nodelist();
		ofs<<v.size()<<std::endl;
		std::map<T::graphnode *, int> mnodes;
		int i= 0;
		for_each(v.begin(), v.end(), [&](T::graphnode *n){
			mnodes[n] = i++;
		});
		for_each(v.begin(), v.end(), [&](T::graphnode *n){
			ofs<<n.id().length()<<" "<<n.id()<<std::endl;
			ofs<<n.outedges().size()<<std::endl;
			for_each(n.outedges().begin(), n.outedges().end(), [&](T::graphnode *n){
				ofs<<mnodes[n]<<" ";
			});
			ofs<<std::endl;
			saveNode(n, ofs);
			ofs<<std::endl;
		});
	}
	
	template<typename T>
	void saveGraph(std::string file, T &graph){
		std::ofstream ofs(file);
		saveGraph(ofs, graph);
	}
};