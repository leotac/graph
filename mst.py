from gurobipy import *

def euler(nodes, arcs):
    n=len(nodes)

    # Create adjacency list
    adj = {}
    for arc in arcs:
         adj.setdefault(arc[0],[]).append(arc[1])
         adj.setdefault(arc[1],[]).append(arc[0])

    tours={} # Eulerian subtour starting in [node]

    for node in nodes:
            print "--------------------",node
            tour=[node]
            while len(adj[node])>0:
                print node

                i=adj[node][0]
                adj[node].remove(i)
                adj[i].remove(node)
                node=i
                tour.append(node)
                
            print tour
            tours.setdefault(node,[])
            if len(tour)>1:
                tours[node]=tour[:-1]

    print "Tours:",tours


    # Depth-first visit of the Eulerian subtours to build one single Eulerian tour
    visited=dict.fromkeys(nodes,False)
    
    tour=[]
    euler=[]
    stack=[nodes[0]]
    print stack
    while len(stack)>0:
        node=stack.pop()
        euler.append(node)
        if visited[node]==False:
            visited[node]=True
            tour.append(node)
            print node
            stack.extend(tours[node])
    print euler
    print tour
            
                    
                          
                          
            
            
            




class unionfind:
    def __init__(self, nodes):
        self.parent = {}
        self.rank = {}
        self.elements = nodes
        for i in self.elements:
            self.makeset(i)

    def makeset(self,i):
        self.parent[i]=i
        self.rank[i]=0

    def findset(self,i):
        if self.parent[i]!=i:
            self.parent[i] = self.findset(self.parent[i])
        return self.parent[i]

    def join(self,i,j):
        i = self.findset(i)
        j = self.findset(j)
        if i==j:
            return
        if self.rank[i]>self.rank[j]:
            self.parent[j]=i
        else:
            self.parent[i]=j
            if self.rank[i]==self.rank[j]:
                self.rank[j]+=1
            
def kruskal(nodes, arcs, cost):
    n = len(nodes)
    tree = []
    uf = unionfind(nodes)
    
    sorted_arcs = sorted(arcs, key=cost.get)

    size=0
    for cur in sorted_arcs:
        u = cur[0]
        v = cur[1]
        if uf.findset(u)!=uf.findset(v):
            tree.append(cur)
            uf.join(u,v)
            size+=1
            if size==n-1:
                break
    return tree
        

def mst(nodes, arcs, cost):
    return kruskal(nodes, arcs, cost)
    

    
