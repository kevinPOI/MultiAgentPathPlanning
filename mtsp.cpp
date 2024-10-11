#include <iostream>
#include <fstream>
// #include <boost/functional/hash.hpp>
#include <regex>
#include <unordered_set>
#include <queue>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <math.h>
#include <time.h>
using namespace std;
void printfplan(vector<vector<pair<int, int>>> vec){
    cout<<"printing fplan"<<endl;
    for(int i =0;i < vec.size(); i++){
        for(int j = 0; j < vec[i].size(); j++){
            cout<<" ("<<vec[i][j].first<<", "<<vec[i][j].second<<") ";
        }
        cout<<endl;
    }
    cout<<"done printing\n";
}
void print2dvec(vector<vector<int>> vec){
    cout<<"printing 2d vector, size of 2d vec is: "<<vec.size()<<endl;
    for(int i =0;i < vec.size(); i++){
        for(int j = 0; j < vec[i].size(); j++){
            cout<<vec[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"done printing\n";
}
void printvec(vector<int> vec){
    cout<<"printing 1d vector, size of 1d vec is: "<<vec.size()<<endl;
    for(int i =0;i < vec.size(); i++){
            cout<<vec[i]<<" ";
    }
    cout<<"done printing\n";
}
int tspA(vector<vector<int>> cost_mat, int start, vector<int> stops, vector<int>& best_path, int max_len){

    int min_len = INT32_MAX;
    sort(stops.begin(), stops.end());



    do{
        int curr_path_len = 0;
        int curr_pos = start;
        vector<int> remaining_stops(stops);
        int num_of_nodes = 0;
        for(int i = 0; i < stops.size(); i++){
            num_of_nodes += 1;
            int next_pos = stops[i];
            curr_path_len += cost_mat[curr_pos][next_pos] + cost_mat[curr_pos][curr_pos];
            if(curr_path_len > max_len || curr_path_len > min_len){
                curr_path_len = 99999;
                break;
            }
            vector<int>::iterator pos = find(remaining_stops.begin(), remaining_stops.end(), next_pos);
            if (pos != remaining_stops.end()){
                remaining_stops.erase(pos);
                if (remaining_stops.size() == 0){
                    break;
                }
            }
            curr_pos = next_pos;
        }
        if (curr_path_len < min_len){
            min_len = curr_path_len;
            best_path.clear();
            for(int i = 0; i < num_of_nodes; i++){
                best_path.push_back(stops[i]);
            }
        }
        
    }while(next_permutation(stops.begin(), stops.end()));
    best_path.insert(best_path.begin(), start);
    return min_len;
}




// The main recursive method to print all possible strings of length "length"
void perm_with_rep(vector<int> agents,vector<int>& prefix,const int n, const int lenght, vector<vector<int>>& result)
    {
        if (lenght == 1)
            {
                for (int j = 0; j < n; j++){
                    prefix.push_back(agents[j]);
                    result.push_back(prefix);
                    prefix.pop_back();
                }
            }//Base case: lenght = 1, print the string "lenght" times + the remaining letter
        else
            {
                for (int i = 0; i < n; i++){
                    prefix.push_back(agents[i]);
                    perm_with_rep(agents, prefix, n, lenght - 1, result);
                    prefix.pop_back();
                }
            }
    }
void distribute(vector<int> free_nodes, int num_agents, vector<vector<vector<int>>>& arangements){

    //assignments: 2d vector, a permutation of tasks distributed to agenets
    //arrangements: 3d vector of arrangement
    //arrangement: 2d vector, [[tasks for agent1],..[tasks for agentN]]
    vector<vector<int>> assignments;
    int num_nodes = free_nodes.size();
    vector<int> prefix;
    vector<int> agents(num_agents);
    iota(agents.begin(), agents.end(), 0);
    
    perm_with_rep(agents, prefix, num_agents, num_nodes, assignments);
    for(vector<int> assignment: assignments){
        vector<vector<int>> arangement;
        for(int i=0; i<num_agents; i++){
            vector<int> temp;
            arangement.push_back(temp);
        }
        for(int i = 0; i < assignment.size(); i++){
            arangement[assignment[i]].push_back(free_nodes[i]);
        }
        arangements.push_back(arangement);
    }
}
bool comp(int i, int j) { return i < j; }
int mtsp(vector<vector<int>> cost_mat, vector<int> starts, vector<vector<int>>& best_path){
    clock_t t, t0;
    t = clock();
    t0 = clock();
    vector<int> free_nodes(cost_mat.size());
    iota(free_nodes.begin(), free_nodes.end(), 0);
    
    for(int s_node: starts){
        vector<int>::iterator pos = find(free_nodes.begin(), free_nodes.end(), s_node);
        free_nodes.erase(pos);
    }

    vector<vector<vector<int>>> arangements;
    distribute(free_nodes, starts.size(), arangements);
    cout<<"112\n";
    int best_len = INT32_MAX;
    int best_index = 0;
    int num_of_arang = arangements.size();
    int num_of_bots = arangements[0].size();
    int counter = 0;
    

    int i1 = 0;
    int i2 = 0;

    vector<vector<vector<int>>> closed_sub_lists;
    for (int i = 0; i < num_of_bots; i++){
        vector<vector<int>> v;
        closed_sub_lists.push_back(v);
    }

    //create a vector for i to iterate though, so it start calculating from the middle
    vector<int> it_order;
    for(int i = arangements.size()/2; i >= 0; i--){
        it_order.push_back(i);
        it_order.push_back(arangements.size() - i - 1);
    }
    //printvec(it_order);
    t = clock() - t;
    cout<<"initialization took: "<<((float)t)/CLOCKS_PER_SEC<<" seconds\n";
    t = clock();
    //for(int i = arangements.size()/2; i >= 0; i--){
    //for(int i = 0; i <arangements.size(); i++){
    for(int i:it_order){
        //cout<<"i1: "<<i1<<"i2: "<<i2<<endl;
        counter += 1;

        if(counter % 5000 == 0){
            cout<<"MTSP processing, "<<counter<<"/"<<num_of_arang<<" complete\n";
            t = clock() - t;
            cout<<"Last 500 arangements took: "<<((float)t)/CLOCKS_PER_SEC<<" seconds\n";
            t = clock();
            cout<<"best len now is "<<best_len<<endl;
        }else if(counter < 30){
            cout<<"MTSP processing, "<<counter<<"/"<<num_of_arang<<" complete\n";
            t = clock() - t;
            cout<<"last arangement took: "<<((float)t)/CLOCKS_PER_SEC<<" seconds\n";
            t = clock();
        }



        //check from mid to two sides: first half
        vector<vector<int>> path_plan1;
        vector<int> path_len1;
        bool breaked = false;
        bool in_closed = false;
        for(int j = 0; j < arangements[i].size(); j++){

            for(vector<int> sub_list : closed_sub_lists[j]){
                if (includes(arangements[i][j].begin(),arangements[i][j].end(), sub_list.begin(), sub_list.end(), comp)){
                    //cout<<"184 breaked\n";
                    in_closed = true;
                    break;
                }
            }
            if(in_closed){
                breaked = true;
                break;
            }
            if(arangements[i][j].size() != 0){
                vector<int> agent_path;
                int agent_len = tspA(cost_mat, starts[j], arangements[i][j], agent_path, best_len);
                if(agent_len > best_len){
                    closed_sub_lists[j].push_back(arangements[i][j]);
                    breaked = true;
                    break;
                }
                //cout<<"agen_len is"<<agent_len<<endl;
                path_plan1.push_back(agent_path);
                path_len1.push_back(agent_len);
                int x = 1;
                
            }else{
                path_len1.push_back(0);
                vector<int> agent_path;
                path_plan1.push_back(agent_path);
            }
            
            
        }
        if(!breaked){
            float total_len = *max_element(path_len1.begin(), path_len1.end());
            if(total_len < best_len){
                best_len = total_len;
                best_path = path_plan1;
                best_index = counter;
            }
        }


    }
    cout<<"mtsp done best len is "<<best_len<<endl;
    t0 = clock() - t0;
    cout<<"Total time took "<<((float)t0)/CLOCKS_PER_SEC<<" seconds\n";
    cout<<"best index is: "<<best_index<<endl;
    return best_len;

}

vector<vector<int>> mtsp_format(vector<vector<int>> mtsp_plan){
    vector<vector<int>> fplan;
    int num_bot = mtsp_plan.size();
    for(vector<int> path : mtsp_plan){
        vector<int> fpath;
        
        for(int i = 1; i < path.size(); i++){//ommit starting node, assume robots in order from 0 to N
            fpath.push_back(path[i] - num_bot);
        }
        fplan.push_back(fpath);
    }
    
    if(true){
        cout<<"\nMTSP Plan is: \n";
        for(int i = 0; i < fplan.size(); i++){
            cout<<"Robot number "<<i<<" transports packages: {";
            for(int j = 0; j < fplan[i].size(); j++){
                cout<<fplan[i][j]<<", ";
            }
            cout<<"}\n";
        }
    }
    return fplan;
}

// void main(){
//     vector<vector<float>> g1 = {{0, 60, 9999, 10, 9999},
//           {60, 0, 9999, 15, 50},
//           {9999, 9999, 0, 20, 30},
//           {10, 15, 20, 0, 40},
//           {9999, 50, 30, 40, 0}};
//     vector<int> starts = {0, 2};
//     vector<vector<int>> best_path;
//     mtsp(g1, starts, best_path);
//     cout<<"150\n";
//     cout<<"best_path size: "<<best_path.size();
//     print2dvec(best_path);
// }