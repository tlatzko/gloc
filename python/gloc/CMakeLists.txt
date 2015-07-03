#SET(LIBRARY_OUTPUT_PATH )

find_package(Boost COMPONENTS python REQUIRED)
find_package( PythonLibs REQUIRED)
find_package( Numpy )

include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
message("include directorties ${BOOST_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}")

message("I am in example folder")

# custom macro with most of the redundant code for making a python example module
macro( addPythonExe _name _srccpp )
  ADD_EXECUTABLE(${_name} ${_srccpp})
  
  # make the pyd library link against boost_numpy python and boost
  TARGET_LINK_LIBRARIES(${_name} boost_numpy)
  
  # put the example target into a VS solution folder named example (should
  # be a no-op for Linux)
  #SET_PROPERTY(TARGET ${_name} PROPERTY FOLDER "example")
endmacro()

macro( addPythonMod _name _srccpp )
  PYTHON_ADD_MODULE(${_name} ${_srccpp})
  
  # make the pyd library link against boost_numpy python and boost
  TARGET_LINK_LIBRARIES(${_name} boost_numpy )
  
  # put the example target into a VS solution folder named example (should
  # be a no-op for Linux)
  #SET_PROPERTY(TARGET ${_name} PROPERTY FOLDER "example")
endmacro()

message("something weird happend")

addPythonMod(greet greet.cpp)
target_link_libraries(greet gloc)

addPythonMod(gaussian gaussian.cpp)

addPythonMod(gloripex gloripex.cpp)
target_link_libraries(gloripex gloc)

# Python install path
set(PY_PATH ${PYTHON_INSTALL_PATH}/gloc)
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )

enable_testing()
find_file( NOSETESTS_EXECUTABLE nosetests )
add_test( NAME PYTHON
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${NOSETESTS_EXECUTABLE} -v --with-xunit )
