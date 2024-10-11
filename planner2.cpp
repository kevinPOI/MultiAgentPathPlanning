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
#include <cstdlib>

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
	cout<<"printing robot to packages: \n";
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
	cout << "--------------------\n printing goal to packages" << endl;
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
vector<vector<int>> makeBigMat(pair<vector<vector<int>>,vector<vector<int>>> distances){
	vector<vector<int>> robot_packages = distances.first;
	vector<vector<int>> goal_packages = distances.second; 
	int num_robot = robot_packages.size();
	int num_packages = goal_packages.size();

	int mat_size = num_robot + num_packages;
	//initilize bigMat
	vector<vector<int>> bigMat(mat_size);
	for(int i = 0; i < mat_size; i++){
		bigMat[i] = vector<int>(mat_size);
	}
	//cout<<"317\n";
	//populate robot rows
	for(int i = 0; i < num_robot; i++){
		//cout<<i<<endl;
		//robot to itself is 0, to other robot is inf
		for(int j = 0; j < num_robot; j++){
			//cout<<j<<endl;
			if(i == j){
				bigMat[i][j] = 0;
			}else{
				bigMat[i][j] = 99999;
			}
		}
		//robot to packages:
		for(int k = 0; k < num_packages; k++){
			bigMat[i][num_robot + k] = robot_packages[i][k];
		}
	}
	//populate goal to package row
	for(int i = 0; i < num_packages; i++){
		//goal to robot is inf
		for(int j = 0; j < num_robot; j++){
			bigMat[num_robot + i][j] = 99999;
		}
		for(int k = 0; k < num_packages; k++){
			bigMat[num_robot + i][num_robot + k] = goal_packages[i][k];
		}
	}
	return bigMat;
}
vector<vector<pair<int, int>>> final_plan(vector<vector<int>> mtsp_plan, Env* env){
	vector<vector<pair<int, int>>> fplan;
	for(int i = 0; i < mtsp_plan.size(); i++){
		vector<pair<int,int>> botplan;
		Robot* curr_robot = env->robots[i];
		Package* curr_package = env->packages[mtsp_plan[i][0]];
		Package* next_package;
		vector<pair<int,int>> botToStop1 = find_path(curr_robot->pos, curr_package->curr_pos, env);
		botplan.insert(botplan.end(), botToStop1.begin(), botToStop1.end());
		for(int j = 0; j < mtsp_plan[i].size() - 1; j++){
			//cout<<mtsp_plan[i].size() <<endl;
			printvec(mtsp_plan[i]);
			curr_package = env->packages[mtsp_plan[i][j]];
			next_package = env->packages[mtsp_plan[i][j+ 1]];
			vector<pair<int,int>> curr_to_goal = find_path(curr_package->curr_pos, curr_package->goal_pos, env);
			vector<pair<int,int>> goal_to_next = find_path(curr_package->goal_pos, next_package->curr_pos, env);
			botplan.insert(botplan.end(), curr_to_goal.begin(), curr_to_goal.end());
			botplan.insert(botplan.end(), goal_to_next.begin(), goal_to_next.end());

		}
		curr_package = env->packages[mtsp_plan[i][mtsp_plan[i].size()-1]];
		vector<pair<int,int>> curr_to_goal = find_path(curr_package->curr_pos, curr_package->goal_pos, env);
		botplan.insert(botplan.end(), curr_to_goal.begin(), curr_to_goal.end());
		fplan.push_back(botplan);
	}
	return fplan;
}

void writePlan(vector<vector<pair<int,int>>> fplan, Env* env){
	ofstream outfile;
	outfile.open("output.txt");
	int maxStep = 0;
	for(int i = 0; i < fplan.size(); i++){
		if(fplan[i].size() > maxStep){
			maxStep = fplan[i].size();
		}
	}
	cout<<"maxStep is"<<maxStep<<endl;
	for(int i = 0; i < maxStep; i++){
		string line = "";
		for(int j = 0; j < fplan.size(); j++){
			//ith step of jth robot
			if(i < fplan[j].size()){
				line.append(to_string(fplan[j][i].first));
				line.append(",");
				line.append(to_string(fplan[j][i].second));
				line.append(" ");
			}
		}
		line.append("\n");
		outfile<<line;
	}
	outfile.close();
	outfile.open("robotPos.txt");
	for(int i = 0; i < env->robots.size(); i++){
		string line = "";
		line.append((to_string((env->robots)[i]->pos.first)));
		line.append(" ");
		line.append(to_string(env->robots[i]->pos.second));
		line.append("\n");
		outfile<<line;
	}
	outfile.close();
	outfile.open("currPos.txt");
	for(int i = 0; i < env->packages.size(); i++){
		string line = "";
		line.append((to_string((env->packages)[i]->curr_pos.first)));
		line.append(" ");
		line.append(to_string(env->packages[i]->curr_pos.second));
		line.append("\n");
		outfile<<line;
	}
	outfile.close();
	outfile.open("goalPos.txt");
	for(int i = 0; i < env->packages.size(); i++){
		string line = "";
		line.append((to_string((env->packages)[i]->goal_pos.first)));
		line.append(" ");
		line.append(to_string(env->packages[i]->goal_pos.second));
		line.append("\n");
		outfile<<line;
	}
	outfile.close();
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

vector<vector<pair<int,int>>> avoid_collision(vector<vector<pair<int,int>>> full_plan, Env* env)
{
	vector<vector<pair<int,int>>> final_plan;
	
	int max_time = 0;
	for(int i = 0; i < full_plan.size(); i ++)
	{
		if (full_plan[i].size() > max_time)
		{
			max_time = full_plan[i].size();
		}
		vector<pair<int, int>> vec;
		final_plan.push_back(vec);
	}

	for(int tstep = 0; tstep < max_time; tstep ++)
	{
		vector<pair<int, int>> all_positions;
		for(int i = 0; i < full_plan.size(); i++)
		{
			vector<pair<int, int>> robot_plan = full_plan[i];
			if (tstep < robot_plan.size())
			{
				// still within plan length
				all_positions.push_back(robot_plan[tstep]);
			}
			else
			{
				// longer than plan length
				all_positions.push_back(make_pair(-1,-1));
			}
		}
		vector<bool> avoided(all_positions.size(), false);
		for(int i = 0; i < all_positions.size(); i ++)
		{
			bool collided = false;
			int whodidihit;
			for(int j = 0; j < all_positions.size(); j++)
			{
				if (i != j)
				{
					if (all_positions[i] == all_positions[j] && all_positions[i] != make_pair(-1, -1))
					{
						collided = true;
						avoided[j] = true;
						whodidihit = j;
					}
					
				}
			}
			// collided means we take some random action
			if (collided && !avoided[i])
			{	int dX[NUMOFDIRS] = {-1, -1, -1,  0,  0,  1, 1, 1};    
    			int dY[NUMOFDIRS] = {-1,  0,  1, -1,  1, -1, 0, 1};
				pair<int, int> new_pos;
				cout<<"avoided\n";
				while(1)
				{
					int random_action = (int)(NUMOFDIRS * (rand() / double(RAND_MAX)));
					new_pos.first = all_positions[i].first + dX[random_action];
					new_pos.second = all_positions[i].second + dY[random_action];
					if(is_valid_position(new_pos, env) && new_pos != full_plan[whodidihit][tstep - 1] && new_pos != full_plan[whodidihit][tstep + 1])
					{
						break;
					}
				}
				// steps aside for a few time steps
				for (int wait; wait < 50; wait ++ )
				{
					final_plan[i].push_back(new_pos);
				}
				final_plan[i].push_back(all_positions[i]);
			}
			else if(all_positions[i] != make_pair(-1, -1))
			{
				final_plan[i].push_back(all_positions[i]);
			}
		}
	}
	return final_plan;
}

int main(){
	srand(2);
    vector<vector<int>> map;
	int x_size, y_size;
    map = loadMap("map1.txt");
    for(int i = 0; i < map.size(); i++){
        for(int j = 0; j < map[0].size(); j++){
            // cout<<map[i][j];
        }
        // cout<<endl;
    }
	Env* env = make_env(map, 3, 9, 9);
	pair<vector<vector<int>>,vector<vector<int>>> distances = make_graph(env);
	cout<<"394\n";
	vector<vector<int>> cost_mat = makeBigMat(distances);
	vector<int> starts(distances.first.size());
	vector<vector<int>> mtsp_plan;
	iota(starts.begin(), starts.end(), 0);

	int min_cost = mtsp(cost_mat, starts, mtsp_plan);

	cout<<"min cost is: "<<min_cost<<endl;
	vector<vector<int>> mtsp_planf = mtsp_format(mtsp_plan);
	vector<vector<pair<int,int>>> fplan = final_plan(mtsp_planf, env);

	for (vector<pair<int,int>> p: fplan)
	{
		cout << p.size() << " ";
	}
	cout << endl;

	vector<vector<pair<int,int>>> collision_free_plan = avoid_collision(fplan, env);

	// printfplan(collision_free_plan);
	
	for (vector<pair<int,int>> p: collision_free_plan)
	{
		cout << p.size() << " ";
	}
	cout << endl;

	writePlan(collision_free_plan, env);
	return 0;
}
