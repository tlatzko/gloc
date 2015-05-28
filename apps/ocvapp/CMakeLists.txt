project(ocvapps)
include_directories(${EXTRA_INCLUDES})

find_package( OpenCV REQUIRED )
add_executable(hello_cv hello_cv.cpp)
target_link_libraries(hello_cv ${OpenCV_LIBS})


