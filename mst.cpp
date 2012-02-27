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
	vector <int> rank;  //rank for each node
	
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
			
			}
		cout<< "("<<edges[i].head <<","<<edges[i].tail<<"):"<<edges[i].cost<<endl;
	}
	
	return tree;
	
}

struct Node{
	int index;
	int pred;  	//predecessor
	float d;		//distance from spanning tree
	
	vector<Edge*> adj; //ugly. incident edges (with cost)
	
	Node(int i, int p, float dd):index(i),pred(p),d(dd){}
	
	bool operator<(const Node& n)const{return d>n.d;}//smaller = higher priority
	
	};

struct Compare_node_pointers {
    bool operator() ( const Node* first, const Node* second ) const
    {
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
						nodes[current->adj[j]->tail]->pred=i;
						Q.update();
				}
			else if(marked[current->adj[j]->head]==false && nodes[current->adj[j]->head]->d > current->adj[j]->cost)
				{		nodes[current->adj[j]->head]->d=current->adj[j]->cost;
						nodes[current->adj[j]->head]->pred=i;
						Q.update();
				} 
				
			
		}
	
	return tree;

}


void connect_component(int i, int n, vector<vector<float> >& forest, vector<int>& assign, int& component){
	cout<<i<<":"<<assign[i]<<endl;
	if(assign[i]==-1)
		{assign[i]=(component++);
			cout<<"Component++"<<i<<endl;	}
	
	for(int j=0;j<n;j++)//stupid
				{
					if(forest[i][j]>0 and assign[j]!=assign[i])//if connected to j
						{assign[j]=assign[i];
						connect_component(j,n,forest, assign, assign[i]);
					}
			
			}
	
	
	
	}

vector<Edge> boruvka(int n, vector<vector<float> >& cost){
	
	vector<vector<float> > forest(n, vector<float>(n));
	
	
	vector<int> assign(n,-1); //connected components; index of component for each node
	
	vector<Edge> compon_min(); //at 'i': minimum cost edge going out of component 'i'
	vector<float> min_cost;
	vector<int> min_edge_head;
	vector<int> min_edge_tail;
	
	int component=0;
	
	vector<Edge> tree;
	
	while(true)
	{
		
		component=0;	
		assign = vector<int>(n,-1);
		cout<<"asda"<<assign[0]<<endl;
		for(int i=0;i<n;i++){
			connect_component(i,n,forest,assign,component);
			}
		
		cerr<<"Component:"<<component<<endl;
		
		if(component==1)
			break;
		
		min_cost=vector<float>(component,999);
		min_edge_head=vector<int>(component,-1);
		min_edge_tail=vector<int>(component,-1);
		
		for(int i=0;i<n;i++){
					
			for(int j=0;j<n;j++){ //Does not work with arcs with same weight. TODO: use lexicographic order
				if(assign[i]!=assign[j] && cost[i][j]>0 
					&& cost[i][j]<min_cost[assign[i]] ){
					
					min_cost[assign[i]]=cost[i][j];
					min_edge_head[assign[i]]=i;
					min_edge_tail[assign[i]]=j;
					}
				}
		}
		
		
		
		for(int i=0;i<component;i++){
			forest[min_edge_head[i]][min_edge_tail[i]]=forest[min_edge_tail[i]][min_edge_head[i]]=min_cost[i];
			tree.push_back(Edge(min_edge_head[i],min_edge_tail[i],min_cost[i]));
			cout<<"C_i:"<<i<<"("<<min_edge_head[i]<<","<<min_edge_tail[i]<<"):";
			cout<<forest[min_edge_head[i]][min_edge_tail[i]]<<endl;
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
  c[1][2] = c[2][1] = 2.1;
  c[1][3] = c[3][1] = 4;
  c[2][3] = c[3][2] = 3.1;
  c[5][4] = c[4][5] = 1;
  c[3][4] = c[4][3] = 3;
  c[0][5] = c[5][0] = 2;
  
  vector<Edge> tree = kruskal(n, c);
  vector<Edge> tree2 = prim(n,c);
  vector<Edge> tree3 = boruvka(n,c);
  
  return 0;
}
