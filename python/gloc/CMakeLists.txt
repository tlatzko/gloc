#SET(LIBRARY_OUTPUT_PATH )
find_package(Boost COMPONENTS python REQUIRED)
find_package( PythonLibs REQUIRED)
find_package( Numpy )

include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
message("include directorties ${BOOST_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}")


file (GLOB_RECURSE PROJECT_SRCS *.cpp *.cxx *.cc *.c)
file (GLOB_RECURSE PYTHON_FILES *.py)
# FILE(COPY ${PYTHON_FILES} DESTINATION .)

foreach(srcfile ${PROJECT_SRCS})
  get_filename_component(sname ${srcfile} NAME_WE)
  add_library(${sname} SHARED ${srcfile})
  message("libraries ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}")
  target_link_libraries(${sname} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} gloc)
  set_target_properties(${sname} PROPERTIES PREFIX "")
  
  if( WIN32 )
    set_target_properties(${sname} PROPERTIES SUFFIX ".pyd")
  elseif( APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # on mac osx, python cannot import libraries with .dylib extension
    set_target_properties(${sname} PROPERTIES SUFFIX ".so")
  endif()  
endforeach()

set(PY_PATH ${PYTHON_INSTALL_PATH}/gloc)
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )

enable_testing()
find_file( NOSETESTS_EXECUTABLE nosetests )
add_test( NAME PYTHON
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${NOSETESTS_EXECUTABLE} -v --with-xunit )
