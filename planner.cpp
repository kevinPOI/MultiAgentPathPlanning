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
#include "mtsp.cpp"
using namespace std;

#define NUMOFDIRS 8
#define EP 1


struct Robot
{
    pair<int, int> pos;
    bool carry;
    vector<pair<int, int>> path;
    
};

struct Package
{
	pair<int, int> curr_pos;
	bool isCarried;
	bool isDelivered;
	pair<int, int> goal_pos;
};

struct NodePair{ // for mTSP
    pair<int, int> pickup;
    pair<int, int> dropoff;
    vector<pair<int, int>> internal_path;//path from pickup to drop off
    vector<vector<pair<int, int>>> paths_to_next; //path from dropoff to every other pickup

};

struct GraphNode
{
	pair<int, int> pos;
	vector<GraphNode *> nbs;
	int nbs_count = 0;

	// A* stuff
	GraphNode* pred;
	double g_value = INT_MAX;
	double h_value;
	double f_value;
};

bool is_neighbor(GraphNode* node1, GraphNode* node2)
{
	return false;
}

struct Env
{
    vector<vector<int>> map;
	vector<Robot*> robots;
	vector<Package*> packages;
	vector<pair<int, int>> goals;
};

bool is_valid_position(pair<int,int> pos, Env* env)
{
    int xpos = pos.first;
    int ypos = pos.second;

    return xpos < env->map[0].size() && 
			ypos < env->map.size() && 
			env->map[ypos][xpos] == 0;
}

Env* make_env(vector<vector<int>> map, int num_robots, int num_packages, int num_dropoffs)
{
	Env* env = new Env;
	env->map = map;
	for(int i = 0; i < num_robots; i ++)
	{
		while(1)
		{
		int randx = (int)(map[0].size() * (rand() / double(RAND_MAX)));
		int randy = (int)(map.size() * (rand() / double(RAND_MAX)));
		if (is_valid_position(make_pair(randx, randy), env))
		{
			Robot* new_robot = new Robot;
			new_robot->pos = make_pair(randx, randy);
			env->robots.push_back(new_robot);
			cout << "rob x:" <<  randx << "rob y:" << randy << endl;
			break;
		}
		}
	}
	for(int i = 0; i < num_packages; i ++)
	{
		while(1)
		{
		int randx = (int)(map[0].size() * (rand() / double(RAND_MAX)));
		int randy = (int)(map.size() * (rand() / double(RAND_MAX)));
		int randx2 = (int)(map[0].size() * (rand() / double(RAND_MAX)));
		int randy2 = (int)(map.size() * (rand() / double(RAND_MAX)));
		if (is_valid_position(make_pair(randx, randy), env) 
			&& is_valid_position(make_pair(randx2, randy2), env))
		{
			Package* new_package = new Package;
			new_package->curr_pos = make_pair(randx, randy);
			new_package->goal_pos = make_pair(randx2, randy2);
			cout << "p x:" <<  randx << "p y:" << randy << endl;
			cout << "pgoal x:" <<  randx2 << "pgoal y:" << randy2 << endl;
			env->packages.push_back(new_package);
			env->goals.push_back(make_pair(randx2, randy2));
			break;
		}
		}
	}
	return env;
}

double compute_heuristic(int robotx, int roboty, int goalx, int goaly){
  int dx = abs(goalx - robotx);
  int dy = abs(goaly - roboty);
  return max(dx, dy) - min(dx, dy) + sqrt(2) * min(dx, dy);
}

bool is_neighbor(pair<int, int> pos1, pair<int,int> pos2)
{
	return true;

	// return compute_heuristic(pos1.first, pos1.second, pos2.first, pos2.second) < 50;	
}

struct node_cmp
{
   bool operator()( const GraphNode* a, const GraphNode* b ) const 
   {
    return a->f_value > b->f_value;
   }
};

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
 
        if (hash1 != hash2) {
            return hash1 ^ hash2;             
        }     
        // If hash1 == hash2, their XOR is zero.
          return hash1;
    }
};

vector<pair<int, int>> find_path(pair<int, int> start_pos, pair<int, int> goal_pos, Env* env)
{
	int dX[NUMOFDIRS] = {-1, -1, -1,  0,  0,  1, 1, 1};    
    int dY[NUMOFDIRS] = {-1,  0,  1, -1,  1, -1, 0, 1};
	 std::priority_queue<GraphNode*, std::vector<GraphNode*>, node_cmp> open_list;
	unordered_map<pair<int, int>, GraphNode*, hash_pair> unordered_open_list;
	unordered_map<pair<int, int>, GraphNode*, hash_pair> closed_list;

	GraphNode* start = new GraphNode;
	start->pos = start_pos;
	start->g_value = 0;
	start->h_value = compute_heuristic(start->pos.first, start->pos.second, goal_pos.first, goal_pos.second);
	start->f_value = start->g_value + EP * start->h_value;
	start->pred = NULL;
	open_list.push(start);
	unordered_open_list.insert(make_pair(start->pos, start));

	GraphNode* curr_expansion = open_list.top();

	while(curr_expansion->pos != goal_pos && open_list.size() != 0)
	{
		curr_expansion = open_list.top();
        open_list.pop();
        pair<int, int> curr_pair = curr_expansion->pos;
        unordered_open_list.erase(curr_pair);

		closed_list.insert(make_pair(curr_expansion->pos, curr_expansion));

		for (int dir = 0; dir < NUMOFDIRS; dir++)
		{
		int new_x = curr_pair.first + dX[dir];
		int new_y = curr_pair.second + dY[dir];

		pair<int, int>  new_pair = make_pair(new_x, new_y);

		if (is_valid_position(new_pair, env) &&
				closed_list.find(new_pair) == closed_list.end())
				{
				
					auto search = unordered_open_list.find(new_pair);

					// if its not in open list then
					if(search == unordered_open_list.end()){
						GraphNode* temp_node = new GraphNode;
						temp_node->pos = make_pair(new_x, new_y);
						temp_node->g_value = curr_expansion->g_value + 1;
						temp_node->h_value = compute_heuristic(new_x, new_y, goal_pos.first, goal_pos.second);
						temp_node->f_value = temp_node->g_value + EP * temp_node->h_value;
						temp_node->pred = curr_expansion;

						open_list.push(temp_node);
						unordered_open_list.insert(make_pair(temp_node->pos, temp_node));
					}
					// if its in open list then check g value and update
					else
					{
						GraphNode* temp_node = search->second;

						if(temp_node->g_value > curr_expansion->g_value + 1)
						{
							temp_node->g_value = curr_expansion->g_value + 1;
							temp_node->f_value = temp_node->g_value + EP * temp_node->h_value;
							temp_node->pred = curr_expansion;
						}
					}
				}
		}
	}

	GraphNode* curr_back_pointer = curr_expansion;

	vector<pair<int, int>> path;
	while(curr_back_pointer->pred != NULL){
		path.push_back(curr_back_pointer->pos);
		curr_back_pointer = curr_back_pointer->pred;
	}
	// assert(curr_back_pointer == NULL);

	//reverse it
	std::reverse(path.begin(),path.end());

	return path;
}

pair<vector<vector<int>>,vector<vector<int>>>  make_graph(Env* env)
{
	vector<vector<int>> map = env->map; 
	vector<Robot*> robots = env->robots;
	vector<Package*> packages = env->packages;
	vector<pair<int, int>> goals = env->goals;

	vector<vector<int>> robot_packages(robots.size(), vector<int>(packages.size()));   // robots on rows, packages on columns
	vector<vector<int>> goal_packages(goals.size(), vector<int>(packages.size()));   // goal on rows, packages on columns

	for(int i = 0; i < robots.size(); i ++ )
	{
		Robot* curr_robot = robots[i];
		for(int j = 0; j < packages.size(); j ++)
		{
			Package* curr_package = packages[j];

			if (is_neighbor(curr_robot->pos, curr_package->curr_pos))
			{
				vector<pair<int, int>> path	= find_path(curr_robot->pos, curr_package->curr_pos, env);
				int path_len = path.size();
				
				robot_packages[i][j] = path_len;
			}
			else
			{
				robot_packages[i][j] = -1;
			}
			cout << robot_packages[i][j] << " ";
			
		}
		cout << endl;
	}
	cout << "--------------------" << endl;
	for(int i = 0; i < goals.size(); i ++ )
	{
		pair<int, int> curr_goal = goals[i];
		for(int j = 0; j < packages.size(); j ++)
		{
			Package* curr_package = packages[j];
			if (is_neighbor(curr_goal, curr_package->curr_pos))
			{
				vector<pair<int, int>> path	= find_path(curr_goal, curr_package->curr_pos, env);
				int path_len = path.size();
				goal_packages[i][j] = path_len;
			}
			else
			{
				goal_packages[i][j] = -1;
			}
			cout << goal_packages[i][j] << " ";
		}
		cout << endl;
	}


	return make_pair(robot_packages, goal_packages);
}

vector<vector<int>> loadMap(string filepath) {// From HW2
	std::FILE *f = fopen(filepath.c_str(), "r");
	if (f) {
	}
	else {
		printf("Opening file failed! \n");
		throw runtime_error("Opening map file failed!");
	}

	int height, width;
	if (fscanf(f, "height %d\nwidth %d\n", &height, &width) != 2) {
		throw runtime_error("Invalid loadMap parsing map metadata");
	}
	////// Go through file and add to m_occupancy
	//double* map = new double[height*width];
    vector<vector<int>> map(height, vector<int> (width, 0));
	double cx, cy, cz; 
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			char c;
			
			do {
				if (fscanf(f, "%c", &c) != 1) {
					cout<<"FAIL\n\n\n";
				}
			} while (isspace(c));
			//cout<<"x "<<x<<" y "<< y << " "<< c <<endl;
			if (!(c == '0')) { 
				map[y][x] = 1; // Note transposed from visual
			} else {
				map[y][x] = 0;
			}
		}
	}
	fclose(f);
	return map;
}

int main(){
    vector<vector<int>> map;
	int x_size, y_size;
    map = loadMap("map1.txt");
    for(int i = 0; i < map.size(); i++){
        for(int j = 0; j < map[0].size(); j++){
            // cout<<map[i][j];
        }
        // cout<<endl;
    }
	Env* env = make_env(map, 2, 2, 2);
	pair<vector<vector<int>>,vector<vector<int>>> distances = make_graph(env);
	vector<vector<float>> g1 = {{0, 60, 9999, 10, 9999},
          {60, 0, 9999, 15, 50},
          {9999, 9999, 0, 20, 30},
          {10, 15, 20, 0, 40},
          {9999, 50, 30, 40, 0}};
    vector<int> starts = {0, 2};
    vector<vector<int>> best_path;
    mtsp(g1, starts, best_path);
    cout<<"150\n";
    cout<<"best_path size: "<<best_path.size();
    print2dvec(best_path);
	return 0;
}
