from sys import maxsize
from itertools import permutations
V = 5
 
# implementation of traveling Salesman Problem
def travellingSalesmanProblem(graph, s):
    
    # store all vertex apart from source vertex
    vertex = []
    best_path = []
    for i in range(V):
        if i != s:
            vertex.append(i)
 
    # store minimum weight Hamiltonian Cycle
    min_len = 99999
    next_permutation=permutations(vertex)
    for i in next_permutation:
 
        # store current Path weight(cost)
        current_pathweight = 0
 
        # compute current path weight
        k = s
        for j in i:
            current_pathweight += graph[k][j]
            k = j
        #current_pathweight += graph[k][s]
 
        # update minimum
        if current_pathweight < min_len:
            min_len = current_pathweight
            best_path = i
    best_path = list(best_path)
    best_path.insert(0, s)
    return min_len, best_path
 
 

if __name__ == "__main__":
 
    # matrix representation of graph
    graph = [[0, 10, 15, 20], [10, 0, 35, 25],
            [15, 35, 0, 30], [20, 25, 30, 0]]
    g1 = [[0, 60, 9999, 10, 9999],
          [60, 0, 9999, 15, 50],
          [9999, 9999, 0, 20, 30],
          [10, 15, 20, 0, 40],
          [9999, 50, 30, 40, 0]]
    s = 0
    travellingSalesmanProblem(g1, s)