from gurobipy import *
import random
import math
import mst
import os
import string


# Compute minimum-cost perfect matching on a complete graph with even number of nodes
# Via Integer Programming. [Edmond's solves it in polytime..]
def matching(nodes,cost):
     m = Model('matching')
     n = len(nodes)
     if n%2!=0:
          return []

     # Create variables
     x = {}
     for i in nodes:
          for j in nodes:
               if j>i:
                    x[i,j] = m.addVar(vtype=GRB.BINARY, obj=cost[i,j], name='x_%s_%s' % (i, j))
                    
     m.update()
     # Degree constraints
     for i in nodes:
         m.addConstr(quicksum(x[j,k] for j in nodes for k in nodes if (k>j) and (k==i or j==i)) == 1,
                     'degree_%s' % (i))
     
     # Compute optimal solution and build solution
     m.optimize()
     match=[]
     for i in nodes:
          for j in nodes:
             if j>i and x[i,j].x > 0.1:
##                  print i, '->', j, ':', x[i,j].x
                  match.append((i,j))

     return match
  

# Find Eulerian tour on a graph with even-degree nodes
def euler(nodes, arcs):
    n=len(nodes)
        
    # Create adjacency list
    adj = {}
    for arc in arcs:
         if arc[0]!=arc[1]:
             adj.setdefault(arc[0],[]).append(arc[1])
             adj.setdefault(arc[1],[]).append(arc[0])

    for node in nodes:
        if len(adj[node])%2!=0 :
            return [],[]

    # Find Eulerian subtours starting in [node]
    tours={} 
    for node in nodes:
            tour=[node]
            while len(adj[node])>0:
                i=adj[node][0]
                adj[node].remove(i)
                adj[i].remove(node)
                node=i
                tour.append(node)
                
            tours.setdefault(node,[])
            if len(tour)>1:
                tours[node]=tour[:-1] #don't insert the starting node twice - cleaner

    # Depth-first visit of the Eulerian subtours to build one single Eulerian tour
    visited=dict.fromkeys(nodes,False)
    
    # Eulerian tour
    euler=[]
    # Hamiltonian tour (shortcutting)
    tour=[]
    stack=[nodes[0]]
 
    while len(stack)>0:
        node=stack.pop()
        euler.append(node)
        if visited[node]==False:
            visited[node]=True
            tour.append(node)
            stack.extend(tours[node])

    return euler, tour        
       

# Solve TSP via an IP with exponentially-many constraints
def tsp(nodes, arcs, cost):
     
     n=len(nodes)
     arcs = tuplelist(arcs)

     # Create all possible subsets (minus empty and full set)
     subsets = [[nodes[i]  for i in xrange(0,n) if k/2**i%2 == 1] for k in xrange(1,2**n-1)]

     # Create optimization model
     m = Model('tsp')

     # Create variables
     x = {}
     for i,j in arcs:
          x[i,j] = m.addVar(vtype=GRB.BINARY, obj=cost[i,j], name='x_%s_%s' % (i, j))
     m.update()

     # Arc capacity constraints
     for i in nodes:
         m.addConstr(quicksum(x[i,j] for (i,j) in arcs.select(i,'*')) == 1, 'degree_out_%s' % (i))
         m.addConstr(quicksum(x[j,i] for (j,i) in arcs.select('*',i)) == 1, 'degree_in_%s' % (i))

     for k in range(len(subsets)):
         m.addConstr(quicksum(x[i,j] for i in subsets[k] for j in set(nodes).difference(set(subsets[k])) if (i,j) in arcs)>=1, 'cut_out_%s' % (k))	
         m.addConstr(quicksum(x[j,i] for i in subsets[k] for j in set(nodes).difference(set(subsets[k])) if (j,i) in arcs)>=1, 'cut_out_%s' % (k))	                            
     
     
     # Compute optimal solution
     m.optimize()

     # Print and build solution
     if m.status == GRB.status.OPTIMAL:
             print '\nNaive Integer Program\nOptimal tour:'
##             for i,j in arcs:
##                 if x[i,j].x > 0.1:
##                     print i, '->', j, ':', x[i,j].x
             tour = [nodes[0]]
             for i in xrange(n-1):
                             for (a,b) in arcs.select(tour[i],'*'):
                                     if x[a,b].x == 1:
                                             tour.append(b)
                                             break
             print tour
             print 'Cost: ', sum([cost[(tour[i],tour[(i+1)%n])] for i in range(n)])
             return tour
                     
                

# Solve TSP via an IP with exponentially-many constraints
def tsp2(nodes, arcs, cost):
     n=len(nodes)
     arcs = tuplelist(arcs)

     print '\nInteger Program w. on-the-fly cuts generation'
     
     # Create optimization model
     m = Model('tsp')

     # Create variables
     x = {}
     for i,j in arcs:
          x[i,j] = m.addVar(vtype=GRB.BINARY, obj=cost[i,j],
                                    name='x_%s_%s' % (i, j))

     m.update()

     # Degree constraints
     for i in nodes:
         m.addConstr(quicksum(x[i,j] for (i,j) in arcs.select(i,'*')) == 1,
                     'out_%s' % (i))

     for i in nodes:
         m.addConstr(quicksum(x[j,i] for (j,i) in arcs.select('*',i)) == 1,
                     'in_%s' % (i))

     sepObj=0
     while(sepObj<2):

         # Compute optimal solution
         m.optimize()
         s=nodes[0]
         for t in nodes[1:]:
             # Separation problem (min-cut weighted with solution from master)
             sep = Model('separation')

             # Create variables
             alpha = {}
             for i,j in arcs:
                             alpha[i,j] = sep.addVar(lb=0,ub=1, obj=(max(x[i,j].x,x[j,i].x) if (j,i) in arcs else x[i,j].x),
                                    name='alpha_%s_%s' % (i, j))
             pi = {}
             for i in nodes:
                              pi[i] = sep.addVar(obj=0,name='pi_%s'%i)
             sep.update()

             # Potential constraints
             for i,j in arcs:
                 sep.addConstr(alpha[i,j] >= pi[i] - pi[j], 'potentials_%s_%s' % (i,j))
                 sep.addConstr(alpha[i,j] >= pi[j] - pi[i], 'potentials2_%s_%s' % (i,j))
             sep.update()
             
             # Root potential
             sep.addConstr(pi[s] - pi[t]>=1,'root_potential')
             sep.optimize()
             
             sepObj=sep.getObjective().getValue()
##             print '\nSeparation minimum', sepObj
##             for i,j in arcs:
##                 print i, '->', j, ':', alpha[i,j].x
##                             
##             for i in nodes:
##                 print i, ':', pi[i].x	
                             
             if sepObj<2:
                 m.addConstr(quicksum(x[i,j] for (i,j) in arcs if alpha[i,j].x>=1)>=1,
                     'aho_%s_%s' % (s,t))
                 m.update()
                 print '*',
                 break;
             

     # Print solution
     if m.status == GRB.status.OPTIMAL:
             print '\nOptimal tour:'
             tour = [nodes[0]]
             for i in xrange(n-1):
                    for (a,b) in arcs.select(tour[i],'*'):
                              if x[a,b].x == 1:
                                        tour.append(b) #add to the tour the only selected outgoing arc
                                        break
             print tour
             print 'Cost: ', sum([cost[(tour[i],tour[(i+1)%n])] for i in range(n)])
             return tour
                     
                     
# Double-tree 2-approximation algorithm for TSP. Complete Metric Graphs only.
def doubletree(nodes,arcs,cost):
     n=len(nodes)
     tree = mst.kruskal(nodes, arcs, cost)
     adj = {}

     # Create adjacency list
     for arc in tree:
          adj.setdefault(arc[0],[]).append(arc[1])
          adj.setdefault(arc[1],[]).append(arc[0])

     visited = dict.fromkeys(nodes, False)
     stack = [nodes[0]]
     tour = []
     while len(stack)>0:
          node = stack.pop()
          visited[node]=True
          tour.append(node)

          for i in adj[node]:
               if visited[i]!=True:
                    stack.append(i)

     print '\nDouble-tree approximation algorithm\nOptimal tour:'
     print tour
     print 'Cost: ', sum([cost[(tour[i],tour[(i+1)%n])] for i in range(n)])
     return tour

# Christofides 3/2-approximation algorithm for TSP. Complete Metric Graphs only.
def christo(nodes,arcs,cost):
     n=len(nodes)
     print '\nChristofides approximation algorithm'

     tree = mst.mst(nodes, arcs, cost)
     adj = {}

     # Create adjacency list
     for arc in tree:
          adj.setdefault(arc[0],[]).append(arc[1])
          adj.setdefault(arc[1],[]).append(arc[0])

     # Nodes with odd degree
     odds = []
     for node in nodes:
        if len(adj[node])%2!=0 :
            odds.append(node)
     
     # Find minimum-cost perfect matching of nodes with odd degree
     match = matching(odds, cost)
     tree.extend(match)

     # Eulerian tour
     eul,tour = euler(nodes,tree)
     

     print 'Optimal tour:'
     print tour
     print 'Cost: ', sum([cost[(tour[i],tour[(i+1)%n])] for i in range(n)])
     return tour


   
             

def main(n,s=8):
##     nodes = ['Detroit', 'Denver', 'Boston', 'New York', 'Seattle']
##     arcs, cost = multidict({
##       ('Detroit', 'Boston'):   300,
##       ('Detroit', 'New York'):  80,
##       ('Detroit', 'Seattle'):  120,
##       ('Denver',  'Boston'):   120,
##       ('Denver',  'New York'): 100,
##       ('Denver',  'Seattle'):  90,
##       ('New York', 'Boston'):   40,
##       ('New York', 'Detroit'):   40,
##       ('Boston',  'Seattle'):   60,
##       ('Boston',  'Detroit'):   100,
##       ('Boston',  'Denver'):   80,
##       ('Seattle',  'Denver'):   60,
##       ('Detroit', 'Denver'):   70,
##       })

     if n<2:
          return

     random.seed(s)
     x,y={},{}
     nodes,arcs,cost=[],[],{}
     for i in range(n):
             nodes.append(string.letters[i%len(string.letters)]*(1+i/len(string.letters)))
             x[nodes[i]]=random.random()*10.0
             y[nodes[i]]=random.random()*10.0

     for i in range(n):
             for j in range(i+1,n):
                  arcs.append((nodes[i],nodes[j]))
                  arcs.append((nodes[j],nodes[i]))
                  cost[nodes[i],nodes[j]]=cost[nodes[j],nodes[i]]=math.sqrt((x[nodes[i]]-x[nodes[j]])**2 + (y[nodes[i]]-y[nodes[j]])**2)


     if n<15:
          tsp(nodes,arcs, cost)
          tsp2(nodes,arcs, cost)

     doubletree(nodes,arcs, cost)
     christo(nodes,arcs, cost)
     return

if __name__ == '__main__':
	main()
