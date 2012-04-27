#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include "mst.cpp"
#include "tsp.cpp"


int main()
{
  int n = 6;

  vector< vector<float> > c(n, vector<float>(n));

  //c[0][1] = c[1][0] = 1;
  
  
  //Positive costs. If 0 (default value), assume nodes not connected.
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
  
  double_tsp(n,c);
  
  return 0;
}
