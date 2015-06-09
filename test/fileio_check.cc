#include <thread>
#include <memory>
#include <string>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gloc/cuboid.hpp"
#include "gloc/laserxa.hpp"

#define CUBFILE TEST_PATH "20120913_135249.cub"
#define LXAFILE TEST_PATH "20120913_135249.lxa"

using namespace std;


TEST(TestFileio, cuboid){
    string path(CUBFILE);
    gloc::CuboidFile cub(path);
    auto frame = cub.getFrame(10);
    cout<< cub.getFrameCount()<<endl;
}

TEST(TestFileio, laserx){
    string path(LXAFILE);
    gloc::LaserxaFile lxa(path);
    // test read of a dataset
    auto dat = lxa.get_data(10);
    cout<< lxa.size()<<endl;
    // test get direction
    auto direct = gloc::get_direction_from_lxa(lxa);
    cout << direct;
    // resolve the time
    auto time = lxa.get_time_at(10);
}
