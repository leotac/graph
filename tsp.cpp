
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include "mst.cpp"

using namespace std;

vector<Edge> double_tsp(int n, vector<vector<float> >& cost)
{
	vector<Edge> tree = kruskal(n, cost);
	vector<int> adj[n];
	vector<int> ham_tour;
	bool visited[n];
	stack<int> to_visit;
	to_visit.push(0); //first node
	
	for(int i=0; i<(int)tree.size();i++){//N-1
		int h = tree[i].head;
		int t = tree[i].tail;
		adj[h].push_back(t);
		adj[t].push_back(h);
		
		}
	
	cout<<"Tour:"<<endl;
	//depth first search is enough
	while(!to_visit.empty()){
		int top = to_visit.top();
		to_visit.pop();
		visited[top]=true;
		cout<<top<<endl;
		ham_tour.push_back(top);
		for(int i=0;i<(int)adj[top].size();i++){
			int cur=adj[top][i];
			if(visited[cur]!=true)
				to_visit.push(cur);
			
			}
		
		
		}
		


return tree;
};

vector<Edge> christo(int n, vector<vector<float> >& cost)
{
	vector<Edge> tree = kruskal(n, cost);
	vector<int> adj[n];
	vector<int> ham_tour;
	bool visited[n];
	stack<int> to_visit;
	to_visit.push(0); //first node
	
	for(int i=0; i<(int)tree.size();i++){//N-1
		int h = tree[i].head;
		int t = tree[i].tail;
		adj[h].push_back(t);
		adj[t].push_back(h);
		
		}
	
	cout<<"Tour:"<<endl;
	//depth first search is enough
	while(!to_visit.empty()){
		int top = to_visit.top();
		to_visit.pop();
		visited[top]=true;
		cout<<top<<endl;
		ham_tour.push_back(top);
		for(int i=0;i<(int)adj[top].size();i++){
			int cur=adj[top][i];
			if(visited[cur]!=true)
				to_visit.push(cur);
			
			}
		
		
		}
		


return tree;
};

