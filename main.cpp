#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct State
{
    vector<vector<char>> mat;

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

struct Node
{
    State state;
    int score;

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
    vector<vector<char>> mat;
    
    while (getline(file, line)) {
        for (char c : line) {
            if (c == '\n' || c == '\r') continue;
            row.push_back(c);
        }
        mat.push_back(row);
        row.clear();
    }
    
    return State(mat);
}


int main(int argc, char *argv[]) {
    State init_state = get_state_from_file(argv[1]);
    init_state.print();
}