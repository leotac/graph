//TODO: rewrite boruvka and prim. possibly parallelize boruvka

#ifndef _MST_CPP
#define _MST_CPP


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
	int pred;  	   	//predecessor (Prim)
	float d;		//distance from spanning tree (Prim)
	
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

struct Node_priority_queue{		//implements a priority queue for Node* where I can update the heap ('manually')

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
	
	//Build adjacency list
	
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
	
	//Prim
	
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


vector<Edge> boruvka(int n, vector<vector<float> >& cost){ //Boruvka using UnionFind structure
	
	vector<Edge*> compon_min; 	//compon_min[i]: minimum-cost edge going out of component 'i'
	vector<float> min_cost;		//^its cost
	vector<Edge> tree;			//edges in mst
	UnionFind forest(n);
	
	//Build adjacency list
		
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
	
	//Boruvka
	
	int size=0;
	
	while(size < n-1)
	{
			
		vector<int> roots;
		min_cost=vector<float>(n,9999);  //not nice, could be done better?
		compon_min=vector<Edge*>(n);
		
		for(int i=0;i<n;i++){
			int this_comp = forest.find_set(i);
			
			if(i == this_comp) //list of roots of the connected components
				roots.push_back(i);
			
			//cout<<"i: "<<i<<" this_comp:"<<this_comp<<endl;
			
			for(int j=0;j<(int)nodes[i]->adj.size();j++){
				int other= (nodes[i]->adj[j]->head != i)? nodes[i]->adj[j]->head : nodes[i]->adj[j]->tail;
				
				// Consider edge if outgoing from current component and has smaller cost than current best 
				// (or equal and lexicographically smaller)
				if( forest.find_set(other) != this_comp 
					&& (nodes[i]->adj[j]->cost < min_cost[this_comp]
						|| 	(nodes[i]->adj[j]->cost == min_cost[this_comp] 
							&&  ( nodes[i]->adj[j]->head < compon_min[this_comp]->head
									|| (nodes[i]->adj[j]->head == compon_min[this_comp]->head && nodes[i]->adj[j]->tail < compon_min[this_comp]->tail))
						)))
					{
						min_cost[this_comp] = nodes[i]->adj[j]->cost;
						compon_min[this_comp] = nodes[i]->adj[j];
						//cout<<"min for component "<<this_comp<<" is now "<<nodes[i]->adj[j]->cost<<endl;
											
					}	
				
				}
		}
	
		
		for(int i=0;i<(int)roots.size();i++){	
			// Loop on unconnected components (one member for each one, their roots)
			// At the beginning of the loop, for each i it holds: find_set(roots[i])==roots[i], 
			// but this does not hold after join() is called, thus connecting components.
			// => Necessary to do every time "find_set" so not to add twice the same edge in case it was selected by 2 components
			// and, at the same time, build the new connected components
			
			int this_comp = forest.find_set(roots[i]);
			if(forest.find_set(compon_min[this_comp]->head)!=forest.find_set(compon_min[this_comp]->tail)){
				tree.push_back(*compon_min[this_comp]);
				//tree.push_back(Edge(compon_min[this_comp]->head,compon_min[this_comp]->tail,min_cost[this_comp]));
				cout<<"("<<compon_min[this_comp]->head<<","<<compon_min[this_comp]->tail<<"):";
				cout<<min_cost[this_comp]<<endl;
				
				forest.join(compon_min[this_comp]->head,compon_min[this_comp]->tail);
				
				size++;
				//cout<<"Size: "<<size<<endl;
				}
			
			}

	}
	
	return tree;
	
	}


//int main()
//{
  //int n = 6;

  //vector< vector<float> > c(n, vector<float>(n));

  ////c[0][1] = c[1][0] = 1;
  
  
  ////Positive costs. If 0 (default value), assume nodes not connected.
  //c[0][2] = c[2][0] = 3.2;
  //c[0][3] = c[3][0] = 2.2;
  //c[1][2] = c[2][1] = 2;
  //c[1][3] = c[3][1] = 4;
  //c[2][3] = c[3][2] = 3.1;
  //c[5][4] = c[4][5] = 1;
  //c[3][4] = c[4][3] = 3;
  //c[0][5] = c[5][0] = 2;
  
  //cout<<"========== Kruskal =========="<<endl;
  //vector<Edge> tree = kruskal(n, c);
  //cout<<"==========  Prim   =========="<<endl;
  //vector<Edge> tree2 = prim(n,c);
  //cout<<"========== Boruvka =========="<<endl;
  //vector<Edge> tree3 = boruvka(n,c);
  
  //return 0;
//}

#endif
