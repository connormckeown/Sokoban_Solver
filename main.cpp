#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

vector<vector<char>> mat;

struct Vec2d
{
    int x;
    int y;

    Vec2d(){}

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

//score is going to be based off the boxes dists from the goals
struct Node
{
    State state;
    Node* parent;
    string moves;
    int g;

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
        Returns shortest dist from player to the nearest goal
    */
    int manhattan_h() {
        int min_dist = INT8_MAX;
        for (Vec2d goal : state.goals) {
            int curr_dist = dist(state.player, goal);
            if (curr_dist < min_dist) {
                min_dist = curr_dist;
            }
        }
        return min_dist;
    }

    /*
        Returns shortest dist from player to nearest box
    */
    int h2() {
        int min_dist = INT8_MAX;
        for (Vec2d b : state.boxes) {
            int box_dist = dist(b, state.player);
            if (box_dist < min_dist) {
                min_dist = box_dist;
            }
        }
        return min_dist;
    }

    /*
        Returns the heuristics added together as a total score
    */
    int f() {
        return g + max(manhattan_h(), h2());
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
bool compare_nodes(const Node &a, const Node &b) {
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
    Returns a char of the opposite direction
*/
char turnback(char c) {
    return c;
}

/*
    Returns a vector of possible successor Nodes
*/
vector<Node> get_possible_successors(Node* curr) {
    vector<Node> successors;

    int px = curr->state.player.x;
    int py = curr->state.player.y;


    // Checking up
    if (!curr->is_box_coord(Vec2d(px, py-1)) && mat[py-1][px] != '#') {
        Node next = Node(curr->state, curr, 'u');
        next.state.player.y -= 1;
        successors.push_back(next);
    }
    // Checking down
    if (!curr->is_box_coord(Vec2d(px, py+1)) && mat[py+1][px] != '#') {
        Node next = Node(curr->state, curr, 'd');
        next.state.player.y += 1;
        successors.push_back(next);
    }
    // Checking left
    if (!curr->is_box_coord(Vec2d(px-1, py)) && mat[py][px-1] != '#') {
        Node next = Node(curr->state, curr, 'l');
        next.state.player.x -= 1;
        successors.push_back(next);
    }
    // Checking right
    if (!curr->is_box_coord(Vec2d(px+1, py)) && mat[py][px+1] != '#') {
        Node next = Node(curr->state, curr, 'r');
        next.state.player.x += 1;
        successors.push_back(next);
    }

    // Checking up and box
    if (curr->is_box_coord(Vec2d(px, py-1)) && !curr->is_box_coord(Vec2d(px, py-2)) && mat[py-2][px] != '#') {
        Node next = Node(curr->state, curr, 'U');
        
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
    if (curr->is_box_coord(Vec2d(px, py+1)) && !curr->is_box_coord(Vec2d(px, py+2)) && mat[py+2][px] != '#') {
        Node next = Node(curr->state, curr, 'D');
        
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
    if (curr->is_box_coord(Vec2d(px-1, py)) && !curr->is_box_coord(Vec2d(px-2, py)) && mat[py][px-2] != '#') {
        Node next = Node(curr->state, curr, 'L');
        
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
    if (curr->is_box_coord(Vec2d(px+1, py)) && !curr->is_box_coord(Vec2d(px+2, py)) && mat[py][px+2] != '#') {
        Node next = Node(curr->state, curr, 'R');
        
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

/*
    A* search with f(n) = g(n) + h(n)
*/
string astar(Node* root) {
    vector<Node> closed_list, open_list;
    open_list.push_back(*root);
    int size = 1000;

    while(!open_list.empty()) {
        sort_nodes(open_list);  // sort by decreasing f(n)
        Node n = open_list.back();  // n has the lowest f(n)
        open_list.pop_back();
        closed_list.push_back(n);   // move n to closed

        //n.print();

        if (n.goal_test()) {
            cout << "open nodes: " << open_list.size() << endl;
            cout << "closed nodes: " << closed_list.size() << endl;
            return n.moves; // if n is a goal return path
        }     

        // print open and closed lists periodically
        if (open_list.size() > size) {
            cout << "open nodes: " << open_list.size() << ", closed nodes: " << closed_list.size() << endl;
            size += 1000;
        }

        for (Node child : get_possible_successors(&n)) {
            bool node_seen = false;

            //if child in open, continue
            for (Node open_node : open_list) {
                if (compare_nodes(child, open_node)) {  
                    node_seen = true;
                    break;
                }
            }
            
            //if child in closed, continue
            for (Node closed_node : closed_list) {
                if (compare_nodes(child, closed_node)) {
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
    -implement turnback in get_possible_successors
    -change heuristic to prioritize boxes that arent on goals
    -also just change heuristic in general
    -switch all coords to new struct with x and y -- DONE
*/
int main(int argc, char *argv[]) {
    Node* start = new Node(get_state_from_file(argv[1]));
    
    // Timing the search
    auto t1 = chrono::high_resolution_clock::now();
    string solution = astar(start);
    auto t2 = chrono::high_resolution_clock::now();
    float duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();

    cout << "time: " << duration/1000000.0 << " seconds" << endl;
    cout << endl << "solution: " << solution << endl;
}