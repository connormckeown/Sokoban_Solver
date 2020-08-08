#include <iostream>
#include <fstream>
#include <vector>
#include<bits/stdc++.h>

using namespace std;

typedef pair<int, int> coord;
vector<vector<char>> mat;

int distance(int x, int y, int dx, int dy) {
    return abs(dx-x) + abs(dy-y);
}

int distance(coord p, coord dp) {
    return abs(dp.first-p.first) + abs(dp.second-p.second);
}


struct State
{
    vector<coord> boxes;
    vector<coord> goals;
    coord player;

    State(){}

    void print() {
        cout << endl << "Player Position: " << player.first << "," << player.second << endl;

        cout << "Box Locations: ";
        for (coord c : boxes)
            cout << c.first << "," << c.second << "  ";
        cout << endl;

        cout << "Goal Locations: ";
        for (coord c : goals)
            cout << c.first << "," << c.second << "  ";
        cout << endl;
    }
};

//score is going to be based off the boxes distances from the goals
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
        Returns shortest distance from player to the nearest goal
    */
    int manhattan_h() {
        int min_dist = INT8_MIN;
        for (coord g : state.goals) {
            int curr_dist = distance(state.player, g);
            if (curr_dist > min_dist) {
                min_dist = curr_dist;
            }
        }
        return min_dist;
    }

    bool is_box_coord(coord p) {    // To check if the given coordinate corresponds to a box
        for (coord b : state.boxes) 
            if (p == b) return true;
        return false;
    }

    // Check that each goal space is covered by a box
    bool goal_test() {
        bool covered;
        for (coord g : state.goals) {
            covered = false;
            for (coord b : state.boxes) {
                if (b == g) covered = true;
            }
            if (!covered) return false;
        }
        return true;
    }

    void print() {
        state.print();
        cout << "moves = " << moves << endl << "g = " << g << endl << "h = " << manhattan_h() << endl;
    }
};

//insertion sort descending
void sort_nodes(vector<Node> &v) {
    Node key = v[0];
    int j;
    for(int i = 1; i < v.size(); i++) {
      key = v[i];
      j = i;
      while(j > 0 && (v[j-1].g + v[j-1].manhattan_h()) < (key.g + key.manhattan_h())) {
         v[j] = v[j-1];
         j--;
      }
      v[j] = key; 
    }
}


/*
    Returns a vector of possible successor Nodes
*/
vector<Node> get_possible_successors(Node* curr) {
    vector<Node> successors;

    int px = curr->state.player.first;
    int py = curr->state.player.second;


    // Checking up
    if (!curr->is_box_coord(make_pair(py-1, px)) && mat[py-1][px] != '#') {
        Node next = Node(curr->state, curr, 'u');
        next.state.player.second -= 1;
        successors.push_back(next);
    }
    // Checking down
    if (!curr->is_box_coord(make_pair(py+1, px)) && mat[py+1][px] != '#') {
        Node next = Node(curr->state, curr, 'd');
        next.state.player.second += 1;
        successors.push_back(next);
    }
    // Checking left
    if (!curr->is_box_coord(make_pair(py, px-1)) && mat[py][px-1] != '#') {
        Node next = Node(curr->state, curr, 'l');
        next.state.player.first -= 1;
        successors.push_back(next);
    }
    // Checking right
    if (!curr->is_box_coord(make_pair(py, px+1)) && mat[py][px+1] != '#') {
        Node next = Node(curr->state, curr, 'r');
        next.state.player.first += 1;
        successors.push_back(next);
    }

    // Checking up and box
    if (curr->is_box_coord(make_pair(px, py-1)) && !curr->is_box_coord(make_pair(px, py-2)) && mat[py-2][px] != '#') {
        Node next = Node(curr->state, curr, 'U');
        
        // Finding the corresponding box and push it
        for (int i = 0; i < next.state.boxes.size(); i++) {
            if (next.state.boxes[i] == make_pair(px, py-1)) {
                next.state.boxes[i] = make_pair(px, py-2);
                next.state.player.second -= 1;
                successors.push_back(next);
            }
        }
    }

    // Checking down and box
    if (curr->is_box_coord(make_pair(px, py+1)) && !curr->is_box_coord(make_pair(px, py+2)) && mat[py+2][px] != '#') {
        Node next = Node(curr->state, curr, 'D');
        
        // Finding the corresponding box and push it
        for (int i = 0; i < next.state.boxes.size(); i++) {
            if (next.state.boxes[i] == make_pair(px, py+1)) {
                next.state.boxes[i] = make_pair(px, py+2);
                next.state.player.second += 1;
                successors.push_back(next);
            }
        }
    }

    // Checking left and box
    if (curr->is_box_coord(make_pair(px-1, py)) && !curr->is_box_coord(make_pair(px-2, py)) && mat[py][px-2] != '#') {
        Node next = Node(curr->state, curr, 'L');
        
        // Finding the corresponding box and push it
        for (int i = 0; i < next.state.boxes.size(); i++) {
            if (next.state.boxes[i] == make_pair(px-1, py)) {
                next.state.boxes[i] = make_pair(px-2, py);
                next.state.player.first -= 1;
                successors.push_back(next);
            }
        }
    }

    // Checking right and box
    if (curr->is_box_coord(make_pair(px+1, py)) && !curr->is_box_coord(make_pair(px+2, py)) && mat[py][px+2] != '#') {
        Node next = Node(curr->state, curr, 'R');
        
        // Finding the corresponding box and push it
        for (int i = 0; i < next.state.boxes.size(); i++) {
            if (next.state.boxes[i] == make_pair(px+1, py)) {
                next.state.boxes[i] = make_pair(px+2, py);
                next.state.player.first += 1;
                successors.push_back(next);
            }
        }
    }

    return successors;
}

bool nodes_equal(const Node &a, const Node &b) {
    return (a.moves == b.moves);
}

string astar(Node* root) {
    vector<Node> closed_list, open_list;
    open_list.push_back(*root);

    while(!open_list.empty()) {
        sort_nodes(open_list);  // sort by decreasing f(n)
        Node n = open_list.back();  // n has the lowest f(n)
        open_list.pop_back();
        //n.print();  //test

        if (n.goal_test()) return n.moves;    // if n is a goal return path
        closed_list.push_back(n);   // move n to closed

        for (Node np : get_possible_successors(&n)) {

            //if child in OPEN list and new n' is not better, continue
            for (Node new_np : open_list) {
                if (nodes_equal(np, new_np) && ((new_np.g + new_np.manhattan_h()) > (np.g + np.manhattan_h()))) continue;
            }

            //if n' in CLOSED list and new n' is not better, continue
            for (Node new_np : closed_list) {
                if (nodes_equal(np, new_np) && ((new_np.g + new_np.manhattan_h()) > (np.g + np.manhattan_h()))) continue;
            }

            // removes all np from open and closed
            for (int i = 0; i < open_list.size(); i++) {
                if (nodes_equal(np, open_list[i])) {
                    open_list.erase(open_list.begin() + i);
                }
            }
            
            for (int i = 0; i < closed_list.size(); i++) {
                if (nodes_equal(np, closed_list[i])) {
                    closed_list.erase(closed_list.begin() + i);
                }
            }

            open_list.push_back(np);
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
                state.boxes.push_back(make_pair(x, y));
            } else if (c == '@') {
                state.player = make_pair(x, y);
            } else if (c == '.') {
                state.goals.push_back(make_pair(x, y));
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

int main(int argc, char *argv[]) {
    Node* start = new Node(get_state_from_file(argv[1]));
    cout << endl << "-----Start Node-----";
    start->print();
    cout << endl;
   
/*
    cout << "-----Successors-----";
    vector<Node> successors = get_possible_successors(start);
    for (Node n : successors) {
        n.print();
    }
    cout << endl;
*/
    string solution = astar(start);
    cout << endl << "solution: " << solution << endl;
}