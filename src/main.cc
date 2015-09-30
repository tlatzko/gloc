#include <iostream>
#include "gloc/cuboid.hpp"

void push_to_talk(){
    std::cout<< "this is a call from python"<< std::endl;
}

int main(int argc, char *argv[]) {

    gloc::CuboidFile cub;
    std::cout << "Hello World!" << std::endl;
}
