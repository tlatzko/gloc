#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <string>
#include <exception>
#include <iostream>
#include <memory>
#include "catch.hpp"
#include "IterativeLevel0.hpp"


#define CUBFILE TEST_PATH "20120913_135249.cub"
#define LXAFILE TEST_PATH "20120913_135249.lxa"

using namespace gloc;
SCENARIO("We have a cubfile and read Read it with a stream", "[CuboidFile], [channel]"){
  GIVEN("streamreader to the reference of a cubfile"){
    WHEN("stream started "){
      THEN("there should be received frames"){
        StreamReader sr(CUBFILE);
        while(true){
          
          auto fr = sr.get_next();
          if(fr == nullptr){
            break;
          }
        }
        
        
      }
    }
    
  }
}
