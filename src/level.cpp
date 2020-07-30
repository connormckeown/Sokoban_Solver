#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


class Level { 
    public:
        int width;
        int height;
        vector<vector<char>> mat;
        Level(string);
        void print_map();
};

Level::Level(string path) {
    int width = 0, height = 0;
    ifstream file(path);
    string line;
    vector<char> row;
    while (getline(file, line)) {
        for (char c : line) {
            if (c == '\n' || c == '\r') continue;
            row.push_back(c);
        }
        if (row.size() > width) {
            width = row.size();
        }
        height++;
        this->mat.push_back(row);
        row.clear();
    }
    this->width = width;
    this->height = height;
}

void Level::print_map() {
    for (vector<char> row : this->mat) {
        for (char c : row) {
            cout << c;
        }
        cout << endl;
    }
}