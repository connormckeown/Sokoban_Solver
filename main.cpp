#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int, int> coord;


struct State
{
    vector<vector<char>> mat;
    vector<coord> boxes;
    vector<coord> goals;
    coord player;

    State(){}

    State(vector<vector<char>> mat) {
        this->mat = mat;
    }

    void print() {
        for (vector<char> row : this->mat) {
            for (char c : row) {
                cout << c;
            }
            cout << endl;
        }
    }
};

//score is going to be based off the boxes distances from the goals
struct Node
{
    State state;
    int score;
    Node* parent;

    Node(State state) {
        this->state = state;
    }

    Node(State state, int score) {
        this->state = state;
        this->score = score;
    }
};

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
        state.mat.push_back(row);
        row.clear();
    }
    
    return state;
}

/*
    Returns a vector of possible successor Nodes paired with their corresponding move
    NEED TO ADD SCORE TO NEXT NODE VIA DISTANCE CHECKING
*/
vector<pair<Node, char>> get_possible_successors(Node* curr) {
    vector<pair<Node, char>> successors;
    int px = curr->state.player.first;
    int py = curr->state.player.second;


    // Checking up
    if (curr->state.mat[px][py-1] == ' ') {
        Node next = Node(curr->state);
        next.parent = curr;
        next.state.player.second -= 1;
        successors.push_back(make_pair(next, 'u'));
    }
    // Checking down
    if (curr->state.mat[px][py+1] == ' ') {
        Node next = Node(curr->state);
        next.parent = curr;
        next.state.player.second += 1;
        successors.push_back(make_pair(next, 'd'));
    }
    // Checking left
    if (curr->state.mat[px-1][py] == ' ') {
        Node next = Node(curr->state);
        next.parent = curr;
        next.state.player.first -= 1;
        successors.push_back(make_pair(next, 'l'));
    }
    // Checking right
    if (curr->state.mat[px+1][py] == ' ') {
        Node next = Node(curr->state);
        next.parent = curr;
        next.state.player.first += 1;
        successors.push_back(make_pair(next, 'r'));
    }

    // Checking up and box
    if (curr->state.mat[px][py-1] == '$') {
        Node next = Node(curr->state);
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px && c.second == py-1) {
                if (next.state.mat[c.first][c.second-1] != '#') {
                    c.second -= 1;
                }
            }
        }

        next.parent = curr;
        next.state.player.second -= 1;
        successors.push_back(make_pair(next, 'U'));
    }

    // Checking down and box
    if (curr->state.mat[px][py+1] == '$') {
        Node next = Node(curr->state);
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px && c.second == py+1) {
                if (next.state.mat[c.first][c.second+1] != '#') {
                    c.second += 1;
                }
            }
        }

        next.parent = curr;
        next.state.player.second += 1;
        successors.push_back(make_pair(next, 'D'));
    }

    // Checking left and box
    if (curr->state.mat[px-1][py] == '$') {
        Node next = Node(curr->state);
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px-1 && c.second == py) {
                if (next.state.mat[c.first-1][c.second] != '#') {
                    c.first -= 1;
                }
            }
        }

        next.parent = curr;
        next.state.player.first -= 1;
        successors.push_back(make_pair(next, 'L'));
    }

    // Checking right and box
    if (curr->state.mat[px+1][py] == '$') {
        Node next = Node(curr->state);
        
        // Finding the corresponding box and push it
        for (coord c : next.state.boxes) {
            if (c.first == px+1 && c.second == py) {
                if (next.state.mat[c.first+1][c.second] != '#') {
                    c.first += 1;
                }
            }
        }

        next.parent = curr;
        next.state.player.first += 1;
        successors.push_back(make_pair(next, 'R'));
    }

    return successors;
}


string astar(Node curr, string solution) {

    
    return solution;
}


int main(int argc, char *argv[]) {
    State init_state = get_state_from_file(argv[1]);
    init_state.print();

    Node start = Node(init_state, 0);
    
    // Printing initial data
    cout << endl << "Player Position: " << start.state.player.first << "," << start.state.player.second << endl;

    cout << "Box Locations: ";
    for (coord c : start.state.boxes)
        cout << c.first << "," << c.second << "  ";
    cout << endl;

    cout << "Goal Locations:";
    for (coord c : start.state.goals)
        cout << c.first << "," << c.second << "  ";
    cout << endl;

    cout << "Possible successors: ";
    vector<pair<Node, char>> successors = get_possible_successors(&start);
    for (pair<Node, char> nc : successors) {
        cout << nc.second << " ";
    }
    cout << endl << endl;


    //string solution = astar(start, "");
}