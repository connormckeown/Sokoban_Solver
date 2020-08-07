#include <iostream>
#include <fstream>
#include <vector>

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

    void print() {
        state.print();
        cout << "moves = " << moves << endl << "g = " << g << endl << "h = " << manhattan_h() << endl;
    }
};


/*
    Returns a vector of possible successor Nodes
    NEED TO ADD SCORE TO NEXT NODE VIA DISTANCE CHECKING
*/
vector<Node> get_possible_successors(Node* curr) {
    vector<Node> successors;
    int px = curr->state.player.first;
    int py = curr->state.player.second;


    // Checking up
    if (mat[py-1][px] == ' ') {
        Node next = Node(curr->state, curr, 'u');
        next.state.player.second -= 1;
        successors.push_back(next);
    }
    // Checking down
    if (mat[py+1][px] == ' ') {
        Node next = Node(curr->state, curr, 'd');
        next.state.player.second += 1;
        successors.push_back(next);
    }
    // Checking left
    if (mat[py][px-1] == ' ') {
        Node next = Node(curr->state, curr, 'l');
        next.state.player.first -= 1;
        successors.push_back(next);
    }
    // Checking right
    if (mat[py][px+1] == ' ') {
        Node next = Node(curr->state, curr, 'r');
        next.state.player.first += 1;
        successors.push_back(next);
    }

    // Checking up and box
    if (mat[py-1][px] == '$' && mat[py-2][px] == ' ') {
        Node next = Node(curr->state, curr, 'U');
        
        // Find the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px && c.second == py-1) {
                if (mat[c.second-1][c.first] != '#') { // Box can be pushed
                    c.second -= 1;
                    next.state.player.second -= 1;
                    successors.push_back(next);
                }
            }
        }
    }

    // Checking down and box
    if (mat[py+1][px] == '$' && mat[py+2][px] == ' ') {
        Node next = Node(curr->state, curr, 'D');
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px && c.second == py+1) {
                if (mat[c.second+1][c.first] != '#') {
                    c.second += 1;
                    next.state.player.second += 1;
                    successors.push_back(next);
                }
            }
        }
    }

    // Checking left and box
    if (mat[py][px-1] == '$' && mat[py][px-2] == ' ') {
        Node next = Node(curr->state, curr, 'L');
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px-1 && c.second == py) {
                if (mat[c.second][c.first-1] != '#') {
                    c.first -= 1;
                    next.state.player.first -= 1;
                    successors.push_back(next);
                }
            }
        }
    }

    // Checking right and box
    if (mat[py][px+1] == '$' && mat[py][px+2] == ' ') {
        Node next = Node(curr->state, curr, 'R');
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px+1 && c.second == py) {
                if (mat[c.second][c.first+1] != '#') {
                    c.first += 1;
                    next.state.player.first += 1;
                    successors.push_back(next);
                }
            }
        }
    }

    return successors;
}


string astar(Node curr) {
    string solution = "";
    return solution;
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
    //start->state.print();
    start->print();
   
    cout << "Possible successors (moves):" << endl;
    vector<Node> successors = get_possible_successors(start);
    for (Node n : successors) {
        n.print();
    }
    cout << endl << endl;


    //string solution = astar(start, "");
}