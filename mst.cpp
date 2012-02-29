//TODO: rewrite boruvka and prim. possibly parallelize boruvka

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Edge{
	int head, tail;
	float cost;
	
	Edge(int h, int t, float c):head(h),tail(t),cost(c){}
	
	bool operator<(const Edge& e)const {return cost<e.cost; }
};
	

struct UnionFind{
	vector<int> parent; //parent for each node
	vector<int> rank;  //rank for each node
	
	UnionFind(int n): parent(n), rank(n)
	{
		for(int i=0;i<n;i++)
			make_set(i);

		}
		
	void make_set(int i)
	{
		parent[i]=i;
		rank[i]=0;
	}
	
	
	int find_set(int i) 
	{
		if(i!=parent[i])
			parent[i] = find_set(parent[i]); //path compression
		return parent[i];
	}
		
	void join(int i, int j)	
	{
		i=find_set(i);
		j=find_set(j);
		if (rank[i]>rank[j])  //i longer. attach j to i
			parent[j]=i;
		else{
			parent[i]=j; //i not longer. attach i to j
			if(rank[i]==rank[j]) //rank grows if equal length
				rank[j]+=1;
		}

	}
	
};


vector<Edge> kruskal(int n, vector<vector<float> >& cost)
{
	vector<Edge> edges;
	
	for(int i=0; i<n;i++)
		for(int j=i+1;j<n;j++)
			if(cost[i][j]>0)
				edges.push_back(Edge(i,j,cost[i][j]));
	
	vector<Edge> tree;
	UnionFind forest(n); //does make_set for each node
	
	sort(edges.begin(),edges.end());
	
	int size = 0;
	for(int i=0;size<n-1;i++){
		int u = edges[i].head;
		int v = edges[i].tail;
		
		if(forest.find_set(u) != forest.find_set(v)){
			tree.push_back(edges[i]);
			forest.join(u,v);
			size++;
			cout<< "("<<edges[i].head <<","<<edges[i].tail<<"):"<<edges[i].cost<<endl;
			}
		
	}
	
	return tree;
	
}

struct Node{
	int index;
	int pred;  	   	//predecessor or root (in Boruvka)
	float d;		//distance from spanning tree
	
	vector<Edge*> adj; //incident edges
	
	Node(int i, int p, float dd):index(i),pred(p),d(dd){}
	
	bool operator<(const Node& n)const{return d>n.d;}//smaller = higher priority
	
	};

struct Compare_node_pointers {
    bool operator() ( const Node* first, const Node* second ) const {
		if(first->d > second->d)
			return true;
		else
			return false;
      }
 };

struct Node_priority_queue{		//implements a priority queue for Node* where I can update the heap (manually)

	vector<Node*> v;
	
	Node_priority_queue(){}
	
	Node_priority_queue(vector<Node*> ve):v(ve){
		make_heap (v.begin(),v.end(),Compare_node_pointers());
		}
	
	Node* pop(){
		Node* nd= v.front();
		pop_heap (v.begin(),v.end(),Compare_node_pointers()); 	//the front item is moved to the last place
		v.pop_back();											//remove the item in the last place
		return nd;
		}
		
	Node* top(){
		return v.front();
		}	
		
	void push(Node* nd){
		v.push_back(nd); 
		push_heap (v.begin(),v.end(),Compare_node_pointers());
		}
		
	void update(){
		make_heap (v.begin(),v.end(),Compare_node_pointers());	//call make_heap on the vector
		}	
};


vector<Edge> prim(int n, vector<vector<float> >& cost){
		
	Node_priority_queue Q;	
	vector<Node*> nodes;
	vector<bool> marked(n,false); //marked nodes
	vector<Edge> tree;
	
	Node* node = new Node(0,0,0);
	nodes.push_back(node);
	Q.push(node);
	for(int i=1; i<n;i++){
		Node* node = new Node(i,-1,9999);
		nodes.push_back(node);
		Q.push(node);
	}
	
	vector<Edge*> edges;
	for(int i=0; i<n;i++)
		for(int j=i+1;j<n;j++)
			if(cost[i][j]>0){
				Edge* edge = new Edge(i,j,cost[i][j]);
				edges.push_back(edge);
				nodes[i]->adj.push_back(edge);
				nodes[j]->adj.push_back(edge);
				}
	
	for(int i=0;i<n;i++){
		Node* current = Q.pop();
		marked[current->index]=true;	
				
		if(i>0){
			cout<<"("<<current->index<<","<<current->pred<<"):"<<current->d<<endl;
			tree.push_back(Edge(current->index,current->pred,current->d));
		}
			
		for(int j=0;j<(int)current->adj.size();j++)
			if(marked[current->adj[j]->tail]==false && nodes[current->adj[j]->tail]->d > current->adj[j]->cost)
				{		nodes[current->adj[j]->tail]->d=current->adj[j]->cost;
						nodes[current->adj[j]->tail]->pred=current->index;
						Q.update();
				}
			else if(marked[current->adj[j]->head]==false && nodes[current->adj[j]->head]->d > current->adj[j]->cost)
				{		nodes[current->adj[j]->head]->d=current->adj[j]->cost;
						nodes[current->adj[j]->head]->pred=current->index;
						Q.update();
				} 
			
		}
	
	return tree;

}


void connect_component(int i, vector<Node*>& nodes, vector<vector<Edge*> >& mst_edges, int& component){
	if(nodes[i]->pred ==-1){	
			nodes[i]->pred = (component++);
				}
	//cout<<i<<" in component"<<nodes[i]->pred<<endl;	
	for(int j=0;j<(int)mst_edges[i].size();j++){
		
		int other= (mst_edges[i][j]->head != i)? mst_edges[i][j]->head : mst_edges[i][j]->tail;
		if( nodes[other]->pred != nodes[i]->pred){
			nodes[other]->pred = nodes[i]->pred;
			connect_component(other,nodes, mst_edges, nodes[i]->pred);
			}	
		
	}
		
}

vector<Edge> boruvka(int n, vector<vector<float> >& cost){
	
	
	vector<Edge*> compon_min; //at 'i': minimum cost edge going out of component 'i'
	vector<float> min_cost;
	vector<Edge> tree;		//edges in mst forest
	vector<vector<Edge*> > mst_edges(n, vector<Edge*>()); //edges in mst forest
	
	int component = 0; //number of connected components that were found in this round
	
	
	vector<Node*> nodes;
	for(int i=0; i<n;i++){
		Node* node = new Node(i,-1,0);
		nodes.push_back(node);
	}
	
	vector<Edge*> edges;
	for(int i=0; i<n;i++)
		for(int j=i+1;j<n;j++)
			if(cost[i][j]>0){
				Edge* edge = new Edge(i,j,cost[i][j]);
				edges.push_back(edge);
				nodes[i]->adj.push_back(edge);
				nodes[j]->adj.push_back(edge);
				}
	
	UnionFind forest(n);
	int size=0;
	while(size<n-1)
	{
		component = 0;	
		
		for(int i=0; i<n;i++){
				nodes[i]->pred=-1;
		}
		
		for(int i=0;i<n;i++)
			connect_component(i,nodes,mst_edges,component);
			
		
		cerr<<"Components:"<<component<<endl;
		
			
		//if(component==1)
		//	break;
		
		min_cost=vector<float>(component,999);
		compon_min=vector<Edge*>(component);
		
		for(int i=0;i<n;i++){
			int thiscomp= forest.find_set(i);
			int this_comp = nodes[i]->pred;
			cout<<"i: "<<i<<" thiscomp:"<<thiscomp<<endl;
			cout<<"i: "<<i<<" this_comp:"<<this_comp<<endl;
			
			for(int j=0;j<(int)nodes[i]->adj.size();j++){
				int other= (nodes[i]->adj[j]->head != i)? nodes[i]->adj[j]->head  : nodes[i]->adj[j]->tail;
				
				if( nodes[other]->pred != this_comp 
					&& (nodes[i]->adj[j]->cost < min_cost[this_comp]
						|| 	(nodes[i]->adj[j]->cost == min_cost[this_comp] 
							&&  ( nodes[i]->adj[j]->head < compon_min[this_comp]->head
									|| (nodes[i]->adj[j]->head == compon_min[this_comp]->head && nodes[i]->adj[j]->tail < compon_min[this_comp]->tail))
						))
					){
						min_cost[this_comp] = nodes[i]->adj[j]->cost;
						compon_min[this_comp] = nodes[i]->adj[j];
						//cout<<"min for component "<<nodes[i]->pred<<" is now "<<nodes[i]->adj[j]->cost<<endl;
											
					}	
				
				}
		}
		

		
		
		
		for(int i=0;i<component;i++){
			
			//possibility of adding twice the same edge if minimal for both components, but it's not a problem
			mst_edges[compon_min[i]->head].push_back(compon_min[i]);
			mst_edges[compon_min[i]->tail].push_back(compon_min[i]);
			
			
			if(forest.find_set(compon_min[i]->head)!=forest.find_set(compon_min[i]->tail)){
				forest.join(compon_min[i]->head,compon_min[i]->tail);
				tree.push_back(Edge(compon_min[i]->head,compon_min[i]->tail,min_cost[i]));
				cout<<"("<<compon_min[i]->head<<","<<compon_min[i]->tail<<"):";
				cout<<min_cost[i]<<endl;
				size++;
				cout<<"Size: "<<size<<endl;
				}
			
			
			
			}

		}
	
	
	return tree;
	
	}


int main()
{
  int n = 6;

  vector< vector<float> > c(n, vector<float>(n));

  //c[0][1] = c[1][0] = 1;
  
  
  //Avoid same weight. Positive costs. If 0, assume nodes not connected.
  c[0][2] = c[2][0] = 3.2;
  c[0][3] = c[3][0] = 2.2;
  c[1][2] = c[2][1] = 2;
  c[1][3] = c[3][1] = 4;
  c[2][3] = c[3][2] = 3.1;
  c[5][4] = c[4][5] = 1;
  c[3][4] = c[4][3] = 3;
  c[0][5] = c[5][0] = 2;
  
  cout<<"========== Kruskal =========="<<endl;
  vector<Edge> tree = kruskal(n, c);
  cout<<"==========  Prim   =========="<<endl;
  vector<Edge> tree2 = prim(n,c);
  cout<<"========== Boruvka =========="<<endl;
  vector<Edge> tree3 = boruvka(n,c);
  
  return 0;
}
