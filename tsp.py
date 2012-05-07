from gurobipy import *

# Model data

#commodities = ['Pencils', 'Pens']
nodes = ['Detroit', 'Denver', 'Boston', 'New York', 'Seattle']

n=len(nodes)

arcs, cost = multidict({
  ('Detroit', 'Boston'):   300,
  ('Detroit', 'New York'):  80,
  ('Detroit', 'Seattle'):  120,
  ('Denver',  'Boston'):   120,
  ('Denver',  'New York'): 100,
  ('Denver',  'Seattle'):  90,
  ('New York', 'Boston'):   40,
  ('New York', 'Detroit'):   40,
  ('Boston',  'Seattle'):   60,
  ('Boston',  'Detroit'):   100,
  ('Boston',  'Denver'):   80,
  ('Seattle',  'Denver'):   60,
  ('Detroit', 'Denver'):   70,
  })
arcs = tuplelist(arcs)

powerset = xrange(1,2**n-1)
print powerset

subsets=[[nodes[i]  for i in xrange(0,n) if k/2**i%2 == 1] for k in powerset]


# Create optimization model
m = Model('tsp')

# Create variables
x = {}
for i,j in arcs:
     x[i,j] = m.addVar(vtype=GRB.BINARY, obj=cost[i,j],
                               name='x_%s_%s' % (i, j))
     #x[i,j] = m.addVar(lb=0, ub=1, obj=cost[i,j],
                               #name='x_%s_%s' % (i, j))
m.update()

# Arc capacity constraints
for i in nodes:
    m.addConstr(quicksum(x[i,j] for (i,j) in arcs.select(i,'*')) == 1,
                'out_%s' % (i))

for i in nodes:
    m.addConstr(quicksum(x[j,i] for (j,i) in arcs.select('*',i)) == 1,
                'in_%s' % (i))

for k in range(len(subsets)):
    m.addConstr(quicksum(x[i,j] for i in subsets[k] for j in set(nodes).difference(set(subsets[k])) if (i,j) in arcs)>=1,
                'aho_%s' % (k))	
                
for k in range(len(subsets)):
    m.addConstr(quicksum(x[j,i] for i in subsets[k] for j in set(nodes).difference(set(subsets[k])) if (j,i) in arcs)>=1,
                'aho2_%s' % (k))	                


m.update()
m.write('pippo.lp')

constrs = m.getConstrs()

print constrs

# Compute optimal solution
m.optimize()

# Print solution
if m.status == GRB.status.OPTIMAL:
        print '\nOptimal tour:'
        for i,j in arcs:
            if x[i,j].x > 0.1:
                print i, '->', j, ':', x[i,j].x
                
                
if m.status == GRB.status.OPTIMAL:
        print '\nOptimal tour:'
        tour = [nodes[0]]
        for i in xrange(n-1):
			for (a,b) in arcs.select(tour[i],'*'):
				if x[a,b].x == 1:
					tour.append(b)
					break
        print tour     
                

