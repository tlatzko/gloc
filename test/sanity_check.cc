#define CATCH_CONFIG_MAIN
#include "catch.hpp"

int add(int a, int b){
  return a + b;
}

SCENARIO("sanity", "[santest]"){
  GIVEN("nothing that matters"){
    WHEN( "adding two numbers"){
      THEN("The result should be the sum of both"){
        REQUIRE(add(10, 10) == 20);
      }
    }
  }
}
