
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include "mst.cpp"

using namespace std;

vector<Edge> double_tsp(int n, vector<vector<float> >& cost)
{
	vector<Edge> tree = kruskal(n, cost);
	vector<int> adj[n];
	vector<int> ham_tour;
	bool visited[n];
	
	
	for(int i=0; i<(int)tree.size();i++){//N-1
		int h = tree[i].head;
		int t = tree[i].tail;
		adj[h].push_back(t);
		adj[t].push_back(h);
		
		}
		


return tree;
};
