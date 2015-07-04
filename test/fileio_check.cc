#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <memory>
#include <string>

#include "gloc/cuboid.hpp"
#include "gloc/laserxa.hpp"

#define CUBFILE TEST_PATH "20120913_135249.cub"
#define LXAFILE TEST_PATH "20120913_135249.lxa"

using namespace std;


TEST_CASE("Cubfile will be opend", "[cubfile]"){
    string path(CUBFILE);
    gloc::CuboidFile cub(path);
    auto frame = cub.getFrame(10);
    cub.printHeader();
    REQUIRE(cub.getFrameCount() > 0);
    
}


TEST_CASE("Laser xa fileio is tested", "[laserxa]"){
    string path(LXAFILE);
    gloc::LaserxaFile lxa(path);
    // test read of a dataset
    auto dat = lxa.get_data(100);
    REQUIRE(dat.timestamp_ms > 0);
    cout<< lxa.size()<<endl;
    // test get direction
    auto direct = gloc::get_direction_from_lxa(lxa);
    cout << direct;
    // resolve the time
    auto time = lxa.get_time_at(10);
    
    REQUIRE(time > 0);
}
