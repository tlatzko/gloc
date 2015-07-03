#define BOOST_TEST_MODULE TestFileio
#include <boost/test/unit_test.hpp>
#include <memory>
#include <string>

#include "gloc/cuboid.hpp"
#include "gloc/laserxa.hpp"

#define CUBFILE TEST_PATH "20120913_135249.cub"
#define LXAFILE TEST_PATH "20120913_135249.lxa"

using namespace std;


BOOST_AUTO_TEST_CASE(cuboid){
    string path(CUBFILE);
    gloc::CuboidFile cub(path);
    auto frame = cub.getFrame(10);
    cout<< cub.getFrameCount()<<endl;
}

BOOST_AUTO_TEST_CASE(laserx){
    string path(LXAFILE);
    gloc::LaserxaFile lxa(path);
    // test read of a dataset
    auto dat = lxa.get_data(100);
    BOOST_ASSERT(dat.timestamp_ms > 0);
    cout<< lxa.size()<<endl;
    // test get direction
    auto direct = gloc::get_direction_from_lxa(lxa);
    cout << direct;
    // resolve the time
    auto time = lxa.get_time_at(10);
    
    BOOST_ASSERT(time > 0);
}
