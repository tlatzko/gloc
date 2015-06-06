add_subdirectory(lib)

set(PY_PATH ${PYTHON_INSTALL_PATH}/gloc)
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )

enable_testing()
find_file( NOSETESTS_EXECUTABLE nosetests )
add_test( NAME PYTHON
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${NOSETESTS_EXECUTABLE} -v --with-xunit )
