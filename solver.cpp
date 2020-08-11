#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

vector<vector<char>> mat; // Holds the map

struct Vec2d
{
    int x;
    int y;

    Vec2d() {
        x = 0;
        y = 0;
    }

    Vec2d(int x, int y) {
        this->x = x;
        this->y = y;
    }

    bool operator==(const Vec2d& v) const
    {
        return (x == v.x && y == v.y);
    }
};

struct State
{
    vector<Vec2d> boxes;
    vector<Vec2d> goals;
    Vec2d player;

    State() {}

    void print() {
        cout << endl << "Player Position: " << player.x << "," << player.y << endl;

        cout << "Box Locations: ";
        for (Vec2d c : boxes)
            cout << c.x << "," << c.y << "  ";
        cout << endl;

        cout << "Goal Locations: ";
        for (Vec2d c : goals)
            cout << c.x << "," << c.y << "  ";
        cout << endl;
    }
};

int dist(int x, int y, int dx, int dy) {
    return abs(dx-x) + abs(dy-y);
}

int dist(const Vec2d &p, const Vec2d &dp) {
    return abs(dp.x-p.x) + abs(dp.y-p.y);
}

/*
    Hungarian Algorithm for the Min-Cost Matching Heuristic.
    Takes in a vector<vector<double>> holding the costs of each box to each goal.
    Returns the min total distance (cost) of the perfect pairing between boxes and goals.
*/
double min_cost_matching(const vector<vector<double>> &cost) {
    int n = int(cost.size());

    vector<double> u(n);
    vector<double> v(n);
    for (int i = 0; i < n; i++) {
        u[i] = cost[i][0];
        for (int j = 1; j < n; j++) 
            u[i] = min(u[i], cost[i][j]);
    }
    for (int j = 0; j < n; j++) {
        v[j] = cost[0][j] - u[0];
        for (int i = 1; i < n; i++) 
            v[j] = min(v[j], cost[i][j] - u[i]);
    }
    
    vector<int> left = vector<int>(n, -1);
    vector<int> right = vector<int>(n, -1);
    int mated = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (right[j] != -1) continue;
            if (fabs(cost[i][j] - u[i] - v[j]) < 1e-10) {
                left[i] = j;
                right[j] = i;
                mated++;
                break;
            }
        }
    }
    
    vector<double> dist(n);
    vector<int> djk(n);
    vector<int> seen(n);
    
    // loop until left and right paired
    while (mated < n) {
        int s = 0;
        while (left[s] != -1) s++;
        
        // dijkstra
        fill(djk.begin(), djk.end(), -1);
        fill(seen.begin(), seen.end(), 0);
        for (int k = 0; k < n; k++) 
        dist[k] = cost[s][k] - u[s] - v[k];
        
        int j = 0;
        while (true) {
            j = -1;
            for (int k = 0; k < n; k++) {
                if (seen[k]) continue;
                if (j == -1 || dist[k] < dist[j]) j = k;
            }
            seen[j] = 1;
            
            if (right[j] == -1) break;
            
            const int i = right[j];
            for (int k = 0; k < n; k++) {
                if (seen[k]) continue;
                const double new_dist = dist[j] + cost[i][k] - u[i] - v[k];
                if (dist[k] > new_dist) {
                dist[k] = new_dist;
                djk[k] = j;
                }
            }
        }
        
        for (int k = 0; k < n; k++) {
            if (k == j || !seen[k]) continue;
            const int i = right[k];
            v[k] += dist[k] - dist[j];
            u[i] -= dist[k] - dist[j];
        }
        u[s] += dist[j];
        
        // augment step
        while (djk[j] >= 0) {
            const int d = djk[j];
            right[j] = right[d];
            left[right[j]] = j;
            j = d;
        }
        right[j] = s;
        left[s] = j;
        mated++;
    }
    
    double result = 0;
    for (int i = 0; i < n; i++)
        result += cost[i][left[i]];
    
    return result;
}

/*

*/
struct BGNode
{
    Vec2d box;
    Vec2d goal;
    BGNode* parent;
    char move;
    int g;

    BGNode(Vec2d box, Vec2d goal) {
        this->box = box;
        this->goal = goal;
        parent = NULL;
        g = 0;
    }

    BGNode(Vec2d box, Vec2d goal, BGNode* parent, char move) {
        this->box = box;
        this->goal = goal;
        this->parent = parent;
        this->move = move;
        g = parent->g+1;
    }

    bool goal_test() {
        if (box == goal) return true;
        return false;
    }

    bool operator==(const BGNode& bg) const
    {
        return (this->box == bg.box && this->goal == bg.goal);
    }

    int f() {
        return g + dist(box, goal);
    }

    /*
        Returns possible successors after a 'push' of the box in the direction of the goal.
    */
    vector<BGNode> get_possible_successors() {
        vector<BGNode> successors;
        
        // Checking up
        if (mat[box.y-1][box.x] != '#') {
            if (this->move != 'd') {
                BGNode up = BGNode(Vec2d(box.x, box.y-1), goal, this, 'u');
                successors.push_back(up);
            }
        }
        
        // Checking down
        if (mat[box.y+1][box.x] != '#') {
            if (this->move != 'u') {
                BGNode down = BGNode(Vec2d(box.x, box.y+1), goal, this, 'd');
                successors.push_back(down);
            }
        }

        // Checking left
        if (mat[box.y][box.x-1] != '#') {
            if (this->move != 'r') {
                BGNode left = BGNode(Vec2d(box.x-1, box.y), goal, this, 'l');
                successors.push_back(left);
            }
        }

        // Checking right
        if (mat[box.y][box.x+1] != '#') {
            if (this->move != 'l') {
                BGNode right = BGNode(Vec2d(box.x+1, box.y), goal, this, 'r');
                successors.push_back(right);
            }
        }
        return successors;
    }
};

void sort_bgnodes(vector<BGNode> &v) {
    BGNode key = v[0];
    int j;
    for(int i = 1; i < v.size(); i++) {
      key = v[i];
      j = i;
      while(j > 0 && v[j-1].f() < key.f()) {
         v[j] = v[j-1];
         j--;
      }
      v[j] = key;
    }
}

int bg_astar(BGNode* root) {
    vector<BGNode> closed_list, open_list;
    open_list.push_back(*root);
    int shortest_dist = 0;

    while(!open_list.empty()) {
        sort_bgnodes(open_list);  // sort by decreasing f(n)
        BGNode n = open_list.back();  // n has the lowest f(n)
        open_list.pop_back();
        closed_list.push_back(n);   // move n to closed

        if (n.goal_test()) {
            return n.g; // return number of moves in the shortest path aka. g
        }

        for (BGNode child : n.get_possible_successors()) {
            bool node_seen = false;

            //if child in open, dont re-add to open
            for (BGNode open_node : open_list) {
                if (child == open_node) {  
                    node_seen = true;
                    break;
                }
            }
            
            //if child in closed, dont add it to open
            for (BGNode closed_node : closed_list) {
                if (child == closed_node) {
                    node_seen = true;
                    break;
                }
            }

            if (!node_seen) 
                open_list.push_back(child);
        
        }
    }
    return 10000; // no solution, distance is inf
}



struct Node
{
    State state;
    Node* parent;
    string moves;
    int g;

    Node(){};

    Node(State state) {
        this->state = state;
        parent = NULL;
        moves = "";
        g = 0;
    }

    Node(State state, Node* parent) {
        this->state = state;
        this->parent = parent;
        moves = parent->moves;
        g = parent->g + 1;
    }

    Node(State state, Node* parent, char move) {
        this->state = state;
        this->parent = parent;
        moves = parent->moves + move;    // Add new move to moves (aka the path to get to this node)
        g = parent->g + 1;
    }

    /*
        Checks if the current state is in a deadlock
    */
    bool is_deadlock() {
        bool north = true;
        bool south = true;
        bool east = true;
        bool west = true;
        bool in_goal = false;

        // Check if any box is in a corner
        for (Vec2d box : state.boxes) {

            // skip if that box is in a goal
            for (Vec2d goal : state.goals) {
                if (box == goal) in_goal = true;
            }

            if (in_goal) {
                in_goal = false;
                continue;
            }

            if (mat[box.y-1][box.x] != '#') north = false; // north has a wall
            if (mat[box.y+1][box.x] != '#') south = false; // south has a wall
            if (mat[box.y][box.x+1] != '#') east = false; // east has a wall
            if (mat[box.y][box.x-1] != '#') west = false; // west has a wall

            if ((west && north) || (north && east) || (east and south) || (west && south)) {
                return true;
            }
            
            // Reset for next box
            bool north = true;
            bool south = true;
            bool east = true;
            bool west = true;
        }
        return false;
    }

    /*
        Returns shortest dist from player to nearest box
        + distances from all boxes to their nearest goal
    */
    int h() {
        int total_dist = 0;
        vector<vector<double>> cost;

        for (Vec2d box : state.boxes) {
            vector<double> temp;
            for (Vec2d goal : state.goals) {
                BGNode* root = new BGNode(box, goal);
                int shortest_dist = bg_astar(root); // sometimes it can't find any path (10000 for each box,goal)
                //cout << "bg_astar found the shortest distance to be " << shortest_dist << endl;
                temp.push_back(shortest_dist); // push back shortest distance between box and goal (if not found, dist is inf)
            }
            cost.push_back(temp);
        }

        total_dist += min_cost_matching(cost);

        // calc dist between player and nearest box using A*
        int temp_dist = INT_MAX;
        for (Vec2d box : state.boxes) {
            BGNode* root = new BGNode(state.player, box); // change
            int shortest_dist = bg_astar(root); 
            if (shortest_dist < temp_dist) {
                temp_dist = shortest_dist;
            }
        }

        total_dist += temp_dist;
        return total_dist;
    }

    /*
        Returns the heuristics added together as a total score
    */
    int f() {
        return g + h();
    }

    bool is_box_coord(Vec2d p) {    // To check if the given coordinate corresponds to a box
        for (Vec2d b : state.boxes) 
            if (p == b) return true;
        return false;
    }

    // Check that each goal space is covered by a box
    bool goal_test() {
        bool covered;
        for (Vec2d g : state.goals) {
            covered = false;
            for (Vec2d b : state.boxes) {
                if (b == g) covered = true;
            }
            if (!covered) return false;
        }
        return true;
    }

    void print() {
        state.print();
        cout << "moves = " << moves << endl << "g = " << g << endl << "f = " << f() << endl;
    }

    /*
        Returns the last move.
        For the backtrack heuristic.
    */
    char last_move() {
        if (parent)
            return parent->moves[parent->moves.length()-1];
        else
            return ' ';
    }

    /*
        Returns a vector of possible successor Nodes
    */
    vector<Node> get_possible_successors() {
        vector<Node> successors;
        int px = this->state.player.x;
        int py = this->state.player.y;
        char prev_move = this->last_move();

        // Checking up
        if (!this->is_box_coord(Vec2d(px, py-1)) && mat[py-1][px] != '#' && prev_move != 'd') {
            Node next = Node(this->state, this, 'u');
            next.state.player.y -= 1;
            successors.push_back(next);
        }
        // Checking down
        if (!this->is_box_coord(Vec2d(px, py+1)) && mat[py+1][px] != '#' && prev_move != 'u') {
            Node next = Node(this->state, this, 'd');
            next.state.player.y += 1;
            successors.push_back(next);
        }
        // Checking left
        if (!this->is_box_coord(Vec2d(px-1, py)) && mat[py][px-1] != '#' && prev_move != 'r') {
            Node next = Node(this->state, this, 'l');
            next.state.player.x -= 1;
            successors.push_back(next);
        }
        // Checking right
        if (!this->is_box_coord(Vec2d(px+1, py)) && mat[py][px+1] != '#' && prev_move != 'l') {
            Node next = Node(this->state, this, 'r');
            next.state.player.x += 1;
            successors.push_back(next);
        }

        // Checking up and box
        if (this->is_box_coord(Vec2d(px, py-1)) && !this->is_box_coord(Vec2d(px, py-2)) && mat[py-2][px] != '#') {
            Node next = Node(this->state, this, 'U');
            
            // Finding the corresponding box and push it
            for (int i = 0; i < next.state.boxes.size(); i++) {
                if (next.state.boxes[i] == Vec2d(px, py-1)) {
                    next.state.boxes[i] = Vec2d(px, py-2);
                    next.state.player.y -= 1;
                    successors.push_back(next);
                }
            }
        }

        // Checking down and box
        if (this->is_box_coord(Vec2d(px, py+1)) && !this->is_box_coord(Vec2d(px, py+2)) && mat[py+2][px] != '#') {
            Node next = Node(this->state, this, 'D');
            
            // Finding the corresponding box and push it
            for (int i = 0; i < next.state.boxes.size(); i++) {
                if (next.state.boxes[i] == Vec2d(px, py+1)) {
                    next.state.boxes[i] = Vec2d(px, py+2);
                    next.state.player.y += 1;
                    successors.push_back(next);
                }
            }
        }

        // Checking left and box
        if (this->is_box_coord(Vec2d(px-1, py)) && !this->is_box_coord(Vec2d(px-2, py)) && mat[py][px-2] != '#') {
            Node next = Node(this->state, this, 'L');
            
            // Finding the corresponding box and push it
            for (int i = 0; i < next.state.boxes.size(); i++) {
                if (next.state.boxes[i] == Vec2d(px-1, py)) {
                    next.state.boxes[i] = Vec2d(px-2, py);
                    next.state.player.x -= 1;
                    successors.push_back(next);
                }
            }
        }

        // Checking right and box
        if (this->is_box_coord(Vec2d(px+1, py)) && !this->is_box_coord(Vec2d(px+2, py)) && mat[py][px+2] != '#') {
            Node next = Node(this->state, this, 'R');
            
            // Finding the corresponding box and push it
            for (int i = 0; i < next.state.boxes.size(); i++) {
                if (next.state.boxes[i] == Vec2d(px+1, py)) {
                    next.state.boxes[i] = Vec2d(px+2, py);
                    next.state.player.x += 1;
                    successors.push_back(next);
                }
            }
        }
        return successors;
    }
};


/*
    Insertion sort in descending order of f(n)
*/
void sort_nodes(vector<Node> &v) {
    Node key = v[0];
    int j;
    for(int i = 1; i < v.size(); i++) {
      key = v[i];
      j = i;
      while(j > 0 && v[j-1].f() < key.f()) {
         v[j] = v[j-1];
         j--;
      }
      v[j] = key;
    }
}

/*
    Checks to see if all box locations and player location are the same
*/
bool nodes_equal(const Node &a, const Node &b) {
    int correct_boxes = 0;
    bool player = false;
    for (Vec2d abox : a.state.boxes) {
        for (Vec2d bbox : b.state.boxes) {
            if (abox == bbox) { 
                correct_boxes++;
            }
        }
    }
    
    if (a.state.player == b.state.player) {
        player = true;
    }

    return correct_boxes == a.state.boxes.size() && player;
}


/*
    A* search
*/
string astar(Node* root) {
    vector<Node> closed_list, open_list;
    open_list.push_back(*root);
    int nodes_explored = 1;
    chrono::steady_clock::time_point timer = chrono::steady_clock::now();

    while(!open_list.empty()) {
        sort_nodes(open_list);  // sort by decreasing f(n)

        // testing
        /*
        for (Node open : open_list) {
            cout << open.f() << " ";
        }
        cout << endl;
        */

        Node n = open_list.back();  // n has the lowest f(n)
        open_list.pop_back();
        closed_list.push_back(n);   // move n to closed

        if (n.goal_test()) {
            cout << "Solution found... " << nodes_explored << " nodes explored" << endl;
            return n.moves; // if n is a goal return path
        }

        if (n.is_deadlock()) continue; // if n is in a deadlock, dont bother checking its children

        // print searching message periodically
        if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - timer).count() > 5.0) {
            timer = std::chrono::steady_clock::now();
            cout << "...Searching... " << nodes_explored << " nodes explored" << endl;
        }        

        for (Node child : n.get_possible_successors()) {
            bool node_seen = false;
            nodes_explored++;

            //if child in open, dont re-add to open
            //reduce this
            for (Node open_node : open_list) {
                if (nodes_equal(child, open_node)) {  
                    node_seen = true;
                    break;
                }
            }
            
            //if child in closed, dont add it to open
            for (Node closed_node : closed_list) {
                if (nodes_equal(child, closed_node)) {
                    node_seen = true;
                    break;
                }
            }

            if (!node_seen) 
                open_list.push_back(child);
        
        }
    }
    return "NO SOLUTION";
}

/*
    IDA* search
*/
string idastar(Node* root, int T) {
    vector<Node> closed_list, open_list;
    open_list.push_back(*root);
    int nodes_explored = 1;
    int new_T = INT_MAX;
    Node n;
    chrono::steady_clock::time_point timer = chrono::steady_clock::now();

    while(!open_list.empty()) {
        sort_nodes(open_list);  // sort by decreasing f(n)
        n = open_list.back();  // n has the lowest f(n)
        open_list.pop_back();
        closed_list.push_back(n);   // move n to closed

        //cout << "threshold = " << threshold << endl;

        if (n.goal_test()) {
            cout << "Solution found... " << nodes_explored << " nodes explored" << endl;
            return n.moves; // if n is a goal return path
        }

        if (n.is_deadlock()) continue; // if n is in a deadlock, dont bother checking its children

        // print searching message periodically
        if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - timer).count() > 5.0) {
            timer = std::chrono::steady_clock::now();
            cout << "...Searching... " << nodes_explored << " nodes explored" << endl;
        }        

        for (Node child : n.get_possible_successors()) {
            bool node_seen = false;
            nodes_explored++;

            //cout << "child.f() = " << child.f() << endl;
            if (child.f() >= T) { // f(child) >= T -> dont add it to open
                continue;
            }

            if (child.f() < new_T) {  // computing next threshold
                new_T = child.f();
                //cout << "next_threshold = " << next_threshold << endl;
            }

            //if child in open, dont re-add to open
            //reduce this
            for (Node open_node : open_list) {
                if (nodes_equal(child, open_node)) {  
                    node_seen = true;
                    break;
                }
            }

            if (node_seen) continue;
            
            //if child in closed, dont add it to open
            for (Node closed_node : closed_list) {
                if (nodes_equal(child, closed_node)) {
                    node_seen = true;
                    break;
                }
            }

            if (!node_seen) 
                open_list.push_back(child);
        
        }
        //threshold = next_threshold; // setting new threshold
    }

    return idastar(&n, new_T);
}





/*
    Returns the initial state and also fills the global matrix
*/
State get_state_from_file(string path) {
    ifstream file(path);
    string line;
    vector<char> row;
    State state;
    int x = 0;
    int y = 0;

    while (getline(file, line)) {
        for (char c : line) {
            if (c == '\n' || c == '\r') {
                continue;
            } else if (c == '$') {
                state.boxes.push_back(Vec2d(x, y));
            } else if (c == '@') {
                state.player = Vec2d(x, y);
            } else if (c == '.') {
                state.goals.push_back(Vec2d(x, y));
            } else if (c == '*') {
                state.goals.push_back(Vec2d(x, y));
                state.boxes.push_back(Vec2d(x, y));
            } else if (c == '+') {
                state.goals.push_back(Vec2d(x, y));
                state.player = Vec2d(x, y);
            }
            x++;
            row.push_back(c);
        }
        x = 0;
        y++;
        mat.push_back(row);
        row.clear();
    }
    
    return state;
}

/*
    TODO
    -change heuristic to prioritize boxes that arent on goals
    -also just change heuristic in general
    -change h() to be the distance from the boxes to nearest goals + distance from player to nearest goals
*/
int main(int argc, char *argv[]) {
    Node* start = new Node(get_state_from_file(argv[1]));
    
    // Timing the search
    auto t1 = chrono::high_resolution_clock::now();
    //string solution = astar(start);
    string solution = idastar(start, start->h());
    auto t2 = chrono::high_resolution_clock::now();
    float duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    // Computing solution complexity
    int B = 0, P = 0, E = 0;
    for (char c : solution) {
        if (c == 'U' || c == 'D' || c == 'L' || c == 'R')
            B++;
        P++;
    }

    // Finding the number of empty cells in the map
    bool inside = false;
    for (vector<char> row : mat) {
        for (char c : row) {
            if (c == ' ' && inside) {
                E++;
            }
            if (c == '#') {
                inside = !inside;
            }
        }
        inside = false;
    }

    cout << "Time: " << duration/1000000.0 << " seconds" << endl;
    cout << "Solution: " << solution << endl;
    cout << "BP/E Solution Complexity: " << B << "*" << P << "/" << E << endl;
}