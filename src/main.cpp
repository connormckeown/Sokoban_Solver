#include "level.cpp"
#include <iostream>
#include <vector>

int main() {
    Level l = Level("resources/levels/level_1.txt");
    l.print_map();
    cout << "l_width = " << l.width << endl;
    cout << "l_height = " << l.height << endl;
}