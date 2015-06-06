#include <thread>
#include <memory>
#include <string>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gloc/cuboid.hpp"


#define CUBFILE TEST_PATH "20120913_135249.cub"
#define LXAFILE TEST_PATH "20120913_135249.lxa"

using namespace std;


TEST(TestFileop, cuboid){
    string path(CUBFILE);
    gloc::CuboidFile cub(path);
    auto frame = cub.getFrame(10);
    cout<< cub.getFrameCount()<<endl;
}
