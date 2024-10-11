from sys import maxsize
from itertools import permutations
import itertools
import numpy as np
V = 5
 
# implementation of traveling Salesman Problem
def tspA(graph, start, stops):
    
    # store all vertex apart from source vertex
    vertex = []
    best_path = []
    for i in stops:
        if i != start:
            vertex.append(i)
    assert(not (start in stops))
    # store minimum weight Hamiltonian Cycle
    min_len = 99999
    next_permutation=permutations(vertex)
    for perm in next_permutation:
 
        # store current Path weight(cost)
        current_pathweight = 0
 
        # set current node to start
        curr_node = start
        remaining_stops = stops.copy()
        num_of_nodes = 0
        for next_node in perm:
            num_of_nodes += 1
            current_pathweight += graph[curr_node][next_node]
            if next_node in remaining_stops:
                remaining_stops.remove(next_node)
                if len(remaining_stops) == 0:
                    break
                    pass
            curr_node = next_node 
        #current_pathweight += graph[k][s]
 
        # update minimum
        if current_pathweight < min_len:
            min_len = current_pathweight
            best_path = perm[0:num_of_nodes]
    best_path = list(best_path)
    best_path.insert(0, start)
    return min_len, best_path



def distribute(free_nodes, num_agents):
    assignments = []
    num_nodes = len(free_nodes)
    for p in itertools.product(np.arange(num_agents), repeat=num_nodes):#create a comb-with-rep for all agents with len = num of nodes
        assignments.append(p)
    pop_index = []
    i = 0
    # while i < len(assignment):
    #     print(i)
    #     for node in np.arange(num_agents):
    #         if not (node in assignment[i]):
    #             print("poped, ", len(assignment))
    #             assignment.pop(i)
    #             i -= 1
    #             break
    #     i += 1

    arangements = []
    for assignment in assignments:#map comb of agents to nodes each agent goes to
        arangement = []
        for i in range(num_agents):#initilize sublist for each agent
            arangement.append([])
        for i in range(len(assignment)):#for each agent, append corresponding nodes
            agent_assigned = assignment[i]
            arangement[agent_assigned].append(free_nodes[i])
        arangements.append(arangement)
    return arangements


    pass


def mtsp(graph, s):
    s0 = s[0]
    s1 = s[1]
    free_nodes = list(np.arange(graph.shape[0]))
    for s_node in s:
        free_nodes.remove(s_node)
    arangements = distribute(free_nodes, len(s))
    best_path = []
    best_len = 999999
    best_arangement = []
    for i in range(len(arangements)):
        arangement = arangements[i]
        path_len = []
        path_plan = []
        
        for j in range(len(arangement)):#j is the agent number, s[j] is starting location of agent, arangement[j] is stops that agent take
            if len(arangement[j]) != 0:
                agent_len, agent_path = tspA(graph, s[j], arangement[j])
                path_len.append(agent_len)
                path_plan.append(agent_path)
            else:
                path_len.append(0)
                path_plan.append([])
        total_len = max(path_len)
        if total_len < best_len:
            best_len = total_len
            best_path = path_plan
            best_arangement = arangement
    # for i in range(len(best_path)):
    #     stops = []
    #     best_path
    return best_len, best_path, best_arangement
        

 
 

if __name__ == "__main__":
 
    # matrix representation of graph
    # graph = [[0, 10, 15, 20], [10, 0, 35, 25],
    #         [15, 35, 0, 30], [20, 25, 30, 0]]
    g1 = [[0, 60, 9999, 10, 9999],
          [60, 0, 9999, 15, 50],
          [9999, 9999, 0, 20, 30],
          [10, 15, 20, 0, 40],
          [9999, 50, 30, 40, 0]]
    g2 = [[0, 60, 9999, 10, 9999, 30, 9999],
          [60, 0, 9999, 15, 50, 20, 9999],
          [9999, 9999, 0, 20, 30, 9999, 9999],
          [10, 15, 20, 0, 40, 9999, 9999],
          [9999, 50, 30, 40, 0, 9999, 30],
          [30, 20, 9999, 9999, 9999, 0, 10],
          [9999, 9999, 9999, 9999, 30, 10, 0]]
    g1 = np.asarray(g1)
    g2 = np.asarray(g2)
    # s = 0
    #tspA(g1, 0, [2,3])
    starts = [0,1,2]

    mtsp(g2, starts)
