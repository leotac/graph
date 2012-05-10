from gurobipy import *



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
    

    
