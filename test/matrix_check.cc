#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <string>
#include <exception>
#include <iostream>
#include <memory>
#include "gloc/matrix.hpp"

using namespace gloc;

/** old testcase for matrix
TEST_CASE("This scenario will check the matrix and vector classes"){
  GIVEN("a pointer to a gloc::Matrix"){
    std::shared_ptr<Mat<double>> mat(new Mat<double>(10, 20));
    WHEN("pointer is not forgotten "){
      THEN("it should throw an exception when get_data funtion is called"){
        REQUIRE_THROWS(mat->get_data());
        
      }
    }

    WHEN("matrix forget the data") {
        mat->forget();
        (*mat)[0] = 10.;
        
        THEN("the matrix can be deleted and the data still exists very dangerous"){
          double* data = mat->get_data();
          mat.reset();
          REQUIRE(mat == nullptr);
          REQUIRE(data[0] == 10.);
          delete data;
        }
    }
  }
}

*/

TEST_CASE("A test case for the matrix class"){
  GIVEN("a matrix A"){
    Mat<double> A(10, 10);
    WHEN("write in Matrix"){
      A(3, 3) = 5.;
      THEN("The result should be the same value"){
        REQUIRE(A(3, 3) == 5);
      }
    }
  }
}
